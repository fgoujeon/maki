//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_REGION_HPP
#define AWESM_DETAIL_REGION_HPP

#include "../sm_conf.hpp"
#include "../null_event.hpp"
#include "state_traits.hpp"
#include "sm_path.hpp"
#include "null_state.hpp"
#include "call_member.hpp"
#include "resolve_transition_table.hpp"
#include "transition_table_digest.hpp"
#include "alternative_lazy.hpp"
#include "any_container.hpp"
#include "ignore_unused.hpp"
#include "event_processing_type.hpp"
#include "tlu/apply.hpp"
#include <type_traits>

namespace awesm::detail
{

template<int Index, class TransitionTable>
class region
{
    public:
        template<class Sm, class Context>
        explicit region(Sm& mach, Context& context):
            states_(mach, context),
            actions_(mach, context),
            guards_(mach, context)
        {
        }

        region(const region&) = delete;
        region(region&&) = delete;
        region& operator=(const region&) = delete;
        region& operator=(region&&) = delete;
        ~region() = default;

        //Check whether the given State is the active state type
        template<class State>
        [[nodiscard]] bool is_active_state() const
        {
            constexpr auto given_state_index = detail::tlu::get_index
            <
                state_tuple_t,
                State
            >;
            return active_state_index_ == given_state_index;
        }

        template<class State>
        const auto& get_state() const
        {
            return get<State>(states_);
        }

        template<class SmPath, class Sm, class Event>
        void start(Sm& mach, const Event& event)
        {
            process_event_2<SmPath, detail::event_processing_type::start>(mach, event);
        }

        template<class SmPath, class Sm, class Event>
        void stop(Sm& mach, const Event& event)
        {
            process_event_2<SmPath, detail::event_processing_type::stop>(mach, event);
        }

        template<class SmPath, class Sm, class Event>
        void process_event(Sm& mach, const Event& event)
        {
            process_event_2<SmPath, detail::event_processing_type::event>(mach, event);
        }

    private:
        using unresolved_transition_table_t = TransitionTable;

        using transition_table_digest_t =
            detail::transition_table_digest<unresolved_transition_table_t>
        ;
        using state_tuple_t = typename transition_table_digest_t::state_tuple;
        using action_tuple_t = typename transition_table_digest_t::action_tuple;
        using guard_tuple_t = typename transition_table_digest_t::guard_tuple;

        using initial_state_t = detail::tlu::at<state_tuple_t, 1>; //0 being null_state

        /*
        Calling detail::resolve_transition_table<> isn't free. We need
        alternative_lazy to avoid the call when it's unnecessary (i.e. when
        there's no pattern-source-state in the transition table).
        */
        struct unresolved_transition_table_holder
        {
            template<class = void>
            using type = unresolved_transition_table_t;
        };
        struct resolved_transition_table_holder
        {
            template<class = void>
            using type = detail::resolve_transition_table
            <
                unresolved_transition_table_t,
                state_tuple_t
            >;
        };
        using transition_table_t = detail::alternative_lazy
        <
            transition_table_digest_t::has_source_state_patterns,
            resolved_transition_table_holder,
            unresolved_transition_table_holder
        >;

        template
        <
            class SmPath,
            detail::event_processing_type ProcessingType,
            class Sm,
            class Event
        >
        void process_event_2(Sm& mach, const Event& event)
        {
            using region_path_t = make_region_path_t<SmPath, Index>;

            process_event_in_active_state<region_path_t>(mach, event);

            auto processed = true;
            if constexpr(ProcessingType == detail::event_processing_type::start)
            {
                using fake_row = row<null_state, Event, initial_state_t>;
                processed = process_event_in_row_if_event_matches<region_path_t, fake_row>
                (
                    mach,
                    event
                );
            }
            else if constexpr(ProcessingType == detail::event_processing_type::stop)
            {
                processed = detail::tlu::apply
                <
                    state_tuple_t,
                    stop_helper_holder<region_path_t>::template stop_helper
                >::call(*this, mach, event);
            }
            else if constexpr(ProcessingType == detail::event_processing_type::event)
            {
                processed = process_event_in_transition_table_once<region_path_t>(mach, event);
            }

            //Anonymous transitions
            if constexpr(transition_table_digest_t::has_void_events)
            {
                if(processed)
                {
                    while(process_event_in_transition_table_once<region_path_t>(mach, null_event{})){}
                }
            }
            else
            {
                detail::ignore_unused(processed);
            }
        }

        //Used to call client code
        template<class Sm, class F>
        void safe_call(Sm& mach, F&& callback)
        {
            try
            {
                callback();
            }
            catch(...)
            {
                mach.process_exception(std::current_exception());
            }
        }

        //Try and trigger one transition
        template<class RegionPath, class Sm, class Event>
        bool process_event_in_transition_table_once(Sm& mach, const Event& event)
        {
            return detail::tlu::apply
            <
                transition_table_t,
                transition_table_event_processor_holder<RegionPath>::template transition_table_event_processor
            >::process(*this, mach, event);
        }

        template<class RegionPath>
        struct transition_table_event_processor_holder
        {
            template<class... Rows>
            struct transition_table_event_processor
            {
                template<class Sm, class Event>
                static bool process(region& reg, Sm& mach, const Event& event)
                {
                    return
                    (
                        reg.process_event_in_row_if_event_matches<RegionPath, Rows>
                        (
                            mach,
                            event
                        ) || ...
                    );
                }
            };
        };

        template<class RegionPath, class Row, class Sm, class Event>
        bool process_event_in_row_if_event_matches(Sm& mach, const Event& event)
        {
            using row_event_type = typename Row::event_type;

            constexpr auto event_matches =
                std::is_same_v<Event, row_event_type> ||
                (
                    std::is_same_v<Event, null_event> &&
                    std::is_void_v<row_event_type>
                )
            ;

            if constexpr(event_matches)
            {
                return process_event_in_row<RegionPath, Row>(mach, event);
            }
            else
            {
                return false;
            }
        }

        template<class RegionPath, class Row, class Sm, class Event>
        bool process_event_in_row(Sm& mach, const Event& event)
        {
            using source_state_t = typename Row::source_state_type;
            using guard_t        = typename Row::guard_type;

            //Make sure the transition source state is the active state
            if(!is_active_state<source_state_t>())
            {
                return false;
            }

            if constexpr(std::is_void_v<guard_t>)
            {
                safe_call
                (
                    mach,
                    [&]
                    {
                        process_event_in_row_2<RegionPath, Row>(mach, event);
                    }
                );
                return true;
            }
            else
            {
                auto processed = false;
                safe_call
                (
                    mach,
                    [&]
                    {
                        if
                        (
                            !detail::call_check
                            (
                                &get<guard_t>(guards_),
                                &event
                            )
                        )
                        {
                            return;
                        }

                        processed = true;
                        process_event_in_row_2<RegionPath, Row>(mach, event);
                    }
                );
                return processed;
            }
        }

        template<class RegionPath, class Row, class Sm, class Event>
        void process_event_in_row_2(Sm& mach, const Event& event)
        {
            using source_state_t = typename Row::source_state_type;
            using target_state_t = typename Row::target_state_type;
            using action_t       = typename Row::action_type;

            //VS2017 is stupid
            detail::ignore_unused(mach, event);

            constexpr auto is_internal_transition =
                std::is_void_v<target_state_t>
            ;

            if constexpr(!is_internal_transition)
            {
                if constexpr(tlu::contains<typename Sm::conf_t, sm_options::before_state_transition>)
                {
                    mach.def_.get_object().template before_state_transition
                    <
                        RegionPath,
                        source_state_t,
                        Event,
                        target_state_t
                    >(event);
                }

                if constexpr(state_traits::requires_on_exit_v<source_state_t, Event>)
                {
                    detail::call_on_exit<RegionPath>
                    (
                        get<source_state_t>(states_),
                        mach,
                        event
                    );
                }

                active_state_index_ = detail::tlu::get_index
                <
                    state_tuple_t,
                    target_state_t
                >;
            }

            if constexpr(!std::is_void_v<action_t>)
            {
                detail::call_execute
                (
                    &get<action_t>(actions_),
                    &event
                );
            }

            if constexpr(!is_internal_transition)
            {
                if constexpr(tlu::contains<typename Sm::conf_t, sm_options::before_entry>)
                {
                    mach.def_.get_object().template before_entry
                    <
                        RegionPath,
                        source_state_t,
                        Event,
                        target_state_t
                    >(event);
                }

                if constexpr
                (
                    !is_internal_transition && //VS2017 is stupid
                    state_traits::requires_on_entry_v<target_state_t, Event>
                )
                {
                    detail::call_on_entry<RegionPath>
                    (
                        get<target_state_t>(states_),
                        mach,
                        event
                    );
                }

                if constexpr(tlu::contains<typename Sm::conf_t, sm_options::after_state_transition>)
                {
                    mach.def_.get_object().template after_state_transition
                    <
                        RegionPath,
                        source_state_t,
                        Event,
                        target_state_t
                    >(event);
                }
            }
        }

        template<class RegionPath>
        struct stop_helper_holder
        {
            template<class... States>
            struct stop_helper
            {
                template<class Sm, class Event>
                static bool call(region& reg, Sm& mach, const Event& event)
                {
                    return (reg.stop_2<RegionPath, States>(mach, &event) || ...);
                }
            };
        };

        template<class RegionPath, class State, class Sm, class Event>
        bool stop_2(Sm& mach, const Event* pevent)
        {
            using fake_row = row<State, Event, null_state>;
            return process_event_in_row_if_event_matches<RegionPath, fake_row>
            (
                mach,
                *pevent
            );
        }

        /*
        Call active_state.on_event(event)
        */
        template<class RegionPath, class Sm, class Event>
        void process_event_in_active_state(Sm& mach, const Event& event)
        {
            return detail::tlu::apply
            <
                state_tuple_t,
                active_state_event_processor_holder<RegionPath>::template active_state_event_processor
            >::process(*this, mach, event);
        }

        //Processes internal events against all states
        template<class RegionPath>
        struct active_state_event_processor_holder
        {
            template<class... States>
            struct active_state_event_processor
            {
                template<class Sm, class Event>
                static void process(region& reg, Sm& mach, const Event& event)
                {
                    (
                        reg.process_event_in_state<RegionPath>
                        (
                            get<States>(reg.states_),
                            mach,
                            event
                        ) || ...
                    );
                }
            };
        };

        //Processes internal events against one state
        template<class RegionPath, class State, class Sm, class Event>
        bool process_event_in_state
        (
            State& state,
            Sm& mach,
            const Event& event
        )
        {
            //VS2017 is stupid
            detail::ignore_unused(state, mach, event);

            if constexpr(state_traits::requires_on_event_v<State, Event>)
            {
                if(is_active_state<State>())
                {
                    safe_call
                    (
                        mach,
                        [&]
                        {
                            call_on_event<RegionPath>(state, mach, event);
                        }
                    );
                    return true;
                }
            }
            return false;
        }

        state_tuple_t states_;
        action_tuple_t actions_;
        guard_tuple_t guards_;

        int active_state_index_ = 0;
};

} //namespace

#endif
