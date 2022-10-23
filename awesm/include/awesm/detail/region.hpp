//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_REGION_HPP
#define AWESM_DETAIL_REGION_HPP

#include "../sm_options.hpp"
#include "../none.hpp"
#include "../null_state.hpp"
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

        template<class Sm, class Event = none>
        void start(Sm& mach, const Event& event = {})
        {
            process_event_2<detail::event_processing_type::start>(mach, event);
        }

        template<class Sm, class Event = none>
        void stop(Sm& mach, const Event& event = {})
        {
            process_event_2<detail::event_processing_type::stop>(mach, event);
        }

        template<class Sm, class Event>
        void process_event(Sm& mach, const Event& event)
        {
            process_event_2<detail::event_processing_type::event>(mach, event);
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
            detail::event_processing_type ProcessingType,
            class Sm,
            class Event
        >
        void process_event_2(Sm& mach, const Event& event)
        {
            if constexpr(Sm::configuration_t::has_in_state_internal_transitions())
            {
                process_event_in_active_state(mach, event);
            }

            auto processed = true;
            if constexpr(ProcessingType == detail::event_processing_type::start)
            {
                using fake_row = row<null_state, Event, initial_state_t>;
                processed = transition_table_row_event_processor<fake_row>::process
                (
                    *this,
                    &mach,
                    &event
                );
            }
            else if constexpr(ProcessingType == detail::event_processing_type::stop)
            {
                processed = detail::tlu::apply
                <
                    state_tuple_t,
                    stop_helper
                >::call(*this, mach, event);
            }
            else if constexpr(ProcessingType == detail::event_processing_type::event)
            {
                processed = process_event_in_transition_table_once(mach, event);
            }

            //Anonymous transitions
            if constexpr(transition_table_digest_t::has_none_events)
            {
                if(processed)
                {
                    while(process_event_in_transition_table_once(mach, none{})){}
                }
            }
            else
            {
                detail::ignore_unused(processed);
            }
        }

        //Used to call client code
        template<class Sm, class F>
        void safe_call(Sm& mach, F&& fun)
        {
            try
            {
                fun();
            }
            catch(...)
            {
                mach.conf_.on_exception(std::current_exception());
            }
        }

        //Try and trigger one transition
        template<class Sm, class Event>
        bool process_event_in_transition_table_once(Sm& mach, const Event& event)
        {
            return detail::tlu::apply
            <
                transition_table_t,
                transition_table_event_processor
            >::process(*this, mach, event);
        }

        template<class... Rows>
        struct transition_table_event_processor
        {
            template<class Sm, class Event>
            static bool process(region& reg, Sm& mach, const Event& event)
            {
                return
                (
                    transition_table_row_event_processor<Rows>::process
                    (
                        reg,
                        &mach,
                        &event
                    ) || ...
                );
            }
        };

        //Processes events against one row of the (resolved) transition table
        template<class Row>
        struct transition_table_row_event_processor
        {
            template<class Sm>
            static bool process
            (
                region& reg,
                Sm* pmach,
                const typename Row::event_type* const pevent
            )
            {
                return reg.process_event_in_row<Row>(*pmach, *pevent);
            }

            /*
            This void* trick allows for shorter build times because we save a
            template instantiation.
            Using an if-constexpr in the process() function above is worse as
            well.
            */
            static bool process(region& /*reg*/, void* /*psm_conf*/, const void* /*pevent*/)
            {
                return false;
            }
        };

        template<class Row, class Sm, class Event>
        bool process_event_in_row(Sm& mach, const Event& event)
        {
            using source_state_t = typename Row::source_state_type;
            using target_state_t = typename Row::target_state_type;
            using action_t       = typename Row::action_type;
            using guard_t        = typename Row::guard_type;

            //Make sure the transition source state is the active state
            if(!is_active_state<source_state_t>())
            {
                return false;
            }

            const auto do_transition = [&](auto /*dummy*/)
            {
                constexpr auto is_internal_transition =
                    std::is_same_v<target_state_t, none>
                ;

                if constexpr(!is_internal_transition)
                {
                    if constexpr(tlu::contains<typename Sm::configuration_t, sm_options::before_state_transition>)
                    {
                        mach.def_.get_object().template before_state_transition
                        <
                            Index,
                            source_state_t,
                            Event,
                            target_state_t
                        >(event);
                    }

                    detail::call_on_exit
                    (
                        &get<source_state_t>(states_),
                        &mach,
                        &event,
                        0
                    );

                    active_state_index_ = detail::tlu::get_index
                    <
                        state_tuple_t,
                        target_state_t
                    >;
                }

                if constexpr(!std::is_same_v<action_t, none>)
                {
                    detail::call_execute
                    (
                        &get<action_t>(actions_),
                        &event
                    );
                }

                if constexpr(!is_internal_transition)
                {
                    if constexpr(tlu::contains<typename Sm::configuration_t, sm_options::before_entry>)
                    {
                        mach.def_.get_object().template before_entry
                        <
                            Index,
                            source_state_t,
                            Event,
                            target_state_t
                        >(event);
                    }

                    detail::call_on_entry
                    (
                        &get<target_state_t>(states_),
                        &mach,
                        &event,
                        0
                    );

                    if constexpr(tlu::contains<typename Sm::configuration_t, sm_options::after_state_transition>)
                    {
                        mach.def_.get_object().template after_state_transition
                        <
                            Index,
                            source_state_t,
                            Event,
                            target_state_t
                        >(event);
                    }
                }
            };

            if constexpr(std::is_same_v<guard_t, none>)
            {
                safe_call
                (
                    mach,
                    [&]
                    {
                        do_transition(0);
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
                        do_transition(0);
                    }
                );
                return processed;
            }
        }

        template<class... States>
        struct stop_helper
        {
            template<class Sm, class Event>
            static bool call(region& reg, Sm& mach, const Event& event)
            {
                return (reg.stop_2<States>(mach, &event) || ...);
            }
        };

        template<class State, class Sm, class Event>
        bool stop_2(Sm& mach, const Event* pevent)
        {
            using fake_row = row<State, Event, null_state>;
            return transition_table_row_event_processor<fake_row>::process
            (
                *this,
                &mach,
                pevent
            );
        }

        /*
        Call active_state.on_event(event)
        */
        template<class Sm, class Event>
        void process_event_in_active_state(Sm& mach, const Event& event)
        {
            return detail::tlu::apply
            <
                state_tuple_t,
                active_state_event_processor
            >::process(*this, mach, event);
        }

        //Processes internal events against all states
        template<class... States>
        struct active_state_event_processor
        {
            template<class Sm, class Event>
            static void process(region& reg, Sm& mach, const Event& event)
            {
                (
                    reg.process_event_in_state
                    (
                        &get<States>(reg.states_),
                        &mach,
                        &event
                    ) || ...
                );
            }
        };

        //Processes internal events against one state
        template<class State, class Sm, class Event>
        auto process_event_in_state
        (
            State* pstate,
            Sm* pmach,
            const Event* pevent
        ) -> decltype(std::declval<State>().on_event(*pmach, *pevent), bool())
        {
            if(is_active_state<State>())
            {
                safe_call
                (
                    *pmach,
                    [&]
                    {
                        pstate->on_event(*pmach, *pevent);
                    }
                );
                return true;
            }
            return false;
        }

        //Processes internal events against one state
        template<class State, class Sm, class Event>
        auto process_event_in_state
        (
            State* pstate,
            Sm* pmach,
            const Event* pevent
        ) -> decltype(std::declval<State>().on_event(*pevent), bool())
        {
            if(is_active_state<State>())
            {
                safe_call
                (
                    *pmach,
                    [&]
                    {
                        pstate->on_event(*pevent);
                    }
                );
                return true;
            }
            return false;
        }

        bool process_event_in_state
        (
            void* /*pstate*/,
            void* /*pmach*/,
            const void* /*pevent*/
        )
        {
            return false;
        }

        state_tuple_t states_;
        action_tuple_t actions_;
        guard_tuple_t guards_;

        int active_state_index_ = 0;
};

} //namespace

#endif
