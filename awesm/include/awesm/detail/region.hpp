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

template<class Sm, class RegionPath, class TransitionTable>
class region
{
    public:
        template<class Context>
        explicit region(Sm& mach, Context& context):
            mach_(mach),
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
                state_wrapper_t<Sm, RegionPath, State>
            >;
            return active_state_index_ == given_state_index;
        }

        template<class State>
        const auto& get_state() const
        {
            return get<state_wrapper_t<Sm, RegionPath, State>>(states_);
        }

        template<class Event>
        void start(const Event& event)
        {
            using fake_row = row<null_state, Event, initial_state_t>;
            if constexpr(transition_table_digest_t::has_void_events)
            {
                if(process_event_in_row_if_event_matches<fake_row>(event))
                {
                    do_anonymous_transitions();
                }
            }
            else
            {
                process_event_in_row_if_event_matches<fake_row>(event);
            }
        }

        template<class Event>
        void stop(const Event& event)
        {
            detail::tlu::apply<state_tuple_t, stop_helper>::call(*this, event);
        }

        template<class Event>
        void process_event(const Event& event)
        {
            process_event_in_active_state(event);
            if constexpr(transition_table_digest_t::has_void_events)
            {
                if(process_event_in_transition_table_once(event))
                {
                    do_anonymous_transitions();
                }
            }
            else
            {
                process_event_in_transition_table_once(event);
            }
        }

    private:
        using unresolved_transition_table_t = TransitionTable;

        using transition_table_digest_t =
            detail::transition_table_digest<Sm, RegionPath, unresolved_transition_table_t>
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

        void do_anonymous_transitions()
        {
            while(process_event_in_transition_table_once(null_event{})){}
        }

        //Used to call client code
        template<class F>
        void safe_call(F&& callback)
        {
            try
            {
                callback();
            }
            catch(...)
            {
                mach_.process_exception(std::current_exception());
            }
        }

        //Try and trigger one transition
        template<class Event>
        bool process_event_in_transition_table_once(const Event& event)
        {
            return detail::tlu::apply
            <
                transition_table_t,
                transition_table_event_processor
            >::process(*this, event);
        }

        template<class... Rows>
        struct transition_table_event_processor
        {
            template<class Event>
            static bool process(region& reg, const Event& event)
            {
                return
                (
                    reg.process_event_in_row_if_event_matches<Rows>
                    (
                        event
                    ) || ...
                );
            }
        };

        template<class Row, class Event>
        bool process_event_in_row_if_event_matches(const Event& event)
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
                return process_event_in_row<Row>(event);
            }
            else
            {
                detail::ignore_unused(event);
                return false;
            }
        }

        template<class Row, class Event>
        bool process_event_in_row(const Event& event)
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
                    [&]
                    {
                        process_event_in_row_2<Row>(event);
                    }
                );
                return true;
            }
            else
            {
                auto processed = false;
                safe_call
                (
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
                        process_event_in_row_2<Row>(event);
                    }
                );
                return processed;
            }
        }

        template<class Row, class Event>
        void process_event_in_row_2(const Event& event)
        {
            using source_state_t = typename Row::source_state_type;
            using target_state_t = typename Row::target_state_type;
            using action_t       = typename Row::action_type;

            detail::ignore_unused(event);

            constexpr auto is_internal_transition =
                std::is_void_v<target_state_t>
            ;

            if constexpr(!is_internal_transition)
            {
                if constexpr(tlu::contains<typename Sm::conf_t, sm_options::before_state_transition>)
                {
                    mach_.def_.get_object().template before_state_transition
                    <
                        RegionPath,
                        source_state_t,
                        Event,
                        target_state_t
                    >(event);
                }

                if constexpr(state_traits::requires_on_exit_v<state_wrapper_t<Sm, RegionPath, source_state_t>, Event>)
                {
                    detail::call_on_exit
                    (
                        get<state_wrapper_t<Sm, RegionPath, source_state_t>>(states_),
                        &event
                    );
                }

                active_state_index_ = detail::tlu::get_index
                <
                    state_tuple_t,
                    state_wrapper_t<Sm, RegionPath, target_state_t>
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
                    mach_.def_.get_object().template before_entry
                    <
                        RegionPath,
                        source_state_t,
                        Event,
                        target_state_t
                    >(event);
                }

                if constexpr
                (
                    !is_internal_transition && //for VS2017
                    state_traits::requires_on_entry_v<state_wrapper_t<Sm, RegionPath, target_state_t>, Event>
                )
                {
                    detail::call_on_entry
                    (
                        get<state_wrapper_t<Sm, RegionPath, target_state_t>>(states_),
                        &event
                    );
                }

                if constexpr(tlu::contains<typename Sm::conf_t, sm_options::after_state_transition>)
                {
                    mach_.def_.get_object().template after_state_transition
                    <
                        RegionPath,
                        source_state_t,
                        Event,
                        target_state_t
                    >(event);
                }
            }
        }

        template<class... States>
        struct stop_helper
        {
            template<class Event>
            static bool call(region& reg, const Event& event)
            {
                return (reg.stop_2<States>(&event) || ...);
            }
        };

        template<class State, class Event>
        bool stop_2(const Event* pevent)
        {
            using fake_row = row<State, Event, null_state>;
            return process_event_in_row_if_event_matches<fake_row>
            (
                *pevent
            );
        }

        /*
        Call active_state.on_event(event)
        */
        template<class Event>
        void process_event_in_active_state(const Event& event)
        {
            detail::tlu::apply
            <
                state_tuple_t,
                active_state_event_processor
            >::process(*this, event);
        }

        //Processes internal events against all states
        template<class... States>
        struct active_state_event_processor
        {
            template<class Event>
            static void process(region& reg, const Event& event)
            {
                (
                    reg.process_event_in_state
                    (
                        get<States>(reg.states_),
                        event
                    ) || ...
                );
            }
        };

        //Processes internal events against one state
        template<class State, class Event>
        bool process_event_in_state
        (
            State& state,
            const Event& event
        )
        {
            detail::ignore_unused(state, event);

            if constexpr(state_traits::requires_on_event_v<State, Event>)
            {
                if(is_active_state<State>())
                {
                    safe_call
                    (
                        [&]
                        {
                            call_on_event(state, event);
                        }
                    );
                    return true;
                }
            }
            return false;
        }

        Sm& mach_;
        state_tuple_t states_;
        action_tuple_t actions_;
        guard_tuple_t guards_;

        int active_state_index_ = 0;
};

} //namespace

#endif
