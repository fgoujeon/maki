//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_FSM_HPP
#define FGFSM_FSM_HPP

#include "default_fsm_configuration.hpp"
#include "internal_transition_policy_helper.hpp"
#include "state_transition_policy_helper.hpp"
#include "any.hpp"
#include "none.hpp"
#include "detail/for_each.hpp"
#include "detail/tuple.hpp"
#include "detail/resolve_transition_table.hpp"
#include "detail/transition_table_digest.hpp"
#include "detail/alternative_lazy.hpp"
#include "detail/ignore_unused.hpp"
#include <queue>
#include <functional>
#include <type_traits>

namespace fgfsm
{

template<class TransitionTable, class Configuration = default_fsm_configuration>
class fsm
{
    private:
        static_assert
        (
            std::is_base_of_v<default_fsm_configuration, Configuration>,
            "Given configuration type must inherit from fgfsm::default_fsm_configuration."
        );

        using pre_transition_event_handler = typename Configuration::pre_transition_event_handler;
        using internal_transition_policy = typename Configuration::internal_transition_policy;
        using state_transition_policy = typename Configuration::state_transition_policy;

        using transition_table_digest =
            detail::transition_table_digest<TransitionTable>
        ;
        using state_tuple  = typename transition_table_digest::state_tuple;
        using action_tuple = typename transition_table_digest::action_tuple;
        using guard_tuple  = typename transition_table_digest::guard_tuple;

        /*
        Calling detail::resolve_transition_table<> isn't free. We need
        alternative_lazy to avoid the call when it's unnecessary (i.e. when
        there's no any-start-state in the transition table).
        */
        struct unresolved_transition_table_holder
        {
            template<class = void>
            using type = TransitionTable;
        };
        struct resolved_transition_table_holder
        {
            template<class = void>
            using type = detail::resolve_transition_table
            <
                TransitionTable,
                state_tuple
            >;
        };
        using transition_table_t = detail::alternative_lazy
        <
            transition_table_digest::has_any_start_states,
            resolved_transition_table_holder,
            unresolved_transition_table_holder
        >;

        template
        <
            class Fsm,
            class StartState,
            class Event,
            class TargetState,
            class Action,
            class Guard
        >
        friend class state_transition_policy_helper;

    public:
        template<class Context>
        fsm(Context& context):
            states_(context),
            actions_(context),
            guards_(context),
            pre_transition_event_handler_{context},
            internal_transition_policy_{context},
            state_transition_policy_{context}
        {
        }

        //Check whether the given State is the active state type
        template<class State>
        bool is_active_state() const
        {
            constexpr auto given_state_index = detail::tlu::get_index
            <
                state_tuple,
                State
            >;
            return given_state_index == active_state_index_;
        }

        template<class Event>
        void process_event(const Event& event)
        {
            if constexpr(Configuration::enable_run_to_completion)
            {
                //Queue event processing in case of recursive call
                if(processing_event_)
                {
                    queued_event_processings_.push
                    (
                        [this, event]
                        {
                            process_event_once(event);
                        }
                    );
                    return;
                }

                struct processing_event_guard
                {
                    processing_event_guard(bool& b):
                        b(b)
                    {
                        b = true;
                    }

                    ~processing_event_guard()
                    {
                        b = false;
                    }

                    bool& b;
                };
                auto processing_guard = processing_event_guard{processing_event_};

                process_event_once(event);

                //Process deferred event processings
                while(!queued_event_processings_.empty())
                {
                    queued_event_processings_.front()();
                    queued_event_processings_.pop();
                }
            }
            else
            {
                process_event_once(event);
            }
        }

    private:
        template<class F>
        void visit_active_state(F&& f)
        {
            detail::for_each
            (
                states_,
                [this, &f](auto& s)
                {
                    using state = std::decay_t<decltype(s)>;
                    if(is_active_state<state>())
                        f(s);
                }
            );
        }

        template<class Event>
        void process_event_once(const Event& event)
        {
            pre_transition_event_handler_.on_event(event);

            if constexpr(Configuration::enable_in_state_internal_transitions)
                process_event_in_active_state(event);

            const bool processed = process_event_in_transition_table_once(event);

            //Anonymous transitions
            if constexpr(transition_table_digest::has_none_events)
            {
                if(processed)
                    while(process_event_in_transition_table_once(none{}));
            }
            else
            {
                detail::ignore_unused(processed);
            }
        }

        //Try and trigger one transition
        template<class Event>
        bool process_event_in_transition_table_once(const Event& event)
        {
            return transition_table_event_processor<transition_table_t>::process
            (
                *this,
                event
            );
        }

        template<class TransitionTable2>
        struct transition_table_event_processor;

        template<class Row, class... Rows>
        struct transition_table_event_processor<transition_table<Row, Rows...>>
        {
            using transition_start_state  = typename Row::start_state;
            using transition_event        = typename Row::event;
            using transition_target_state = typename Row::target_state;
            using transition_action       = typename Row::action;
            using transition_guard        = typename Row::guard;

            template<class Event>
            static bool process(fsm& sm, const Event& event)
            {
                //Make sure the event type is the one described the transition
                if constexpr(std::is_same_v<Event, transition_event>)
                {
                    //Make sure the transition start state is the active state
                    if(sm.is_active_state<transition_start_state>())
                    {
                        //Perform the transition
                        using helper_t = state_transition_policy_helper
                        <
                            fsm,
                            transition_start_state,
                            transition_event,
                            transition_target_state,
                            transition_action,
                            transition_guard
                        >;
                        const auto processed =
                            sm.state_transition_policy_.do_transition
                            (
                                helper_t{sm, event}
                            )
                        ;

                        if(processed)
                            return true;
                    }
                }

                //Next row
                if constexpr(sizeof...(Rows) > 0)
                {
                    using next_row_helper_t = transition_table_event_processor
                    <
                        transition_table<Rows...>
                    >;
                    return next_row_helper_t::process(sm, event);
                }
                else
                {
                    detail::ignore_unused(event);
                    return false;
                }
            }
        };

        /*
        Call active_state.on_event(event)
        */
        template<class Event>
        void process_event_in_active_state(const Event& event)
        {
            visit_active_state
            (
                [this, &event](auto& s)
                {
                    using state = std::decay_t<decltype(s)>;
                    auto helper = internal_transition_policy_helper
                    <
                        state,
                        Event
                    >{s, event};
                    internal_transition_policy_.do_transition(helper);
                }
            );
        }

    private:
        state_tuple states_;
        action_tuple actions_;
        guard_tuple guards_;
        pre_transition_event_handler pre_transition_event_handler_;
        internal_transition_policy internal_transition_policy_;
        state_transition_policy state_transition_policy_;

        int active_state_index_ = 0;
        bool processing_event_ = false;
        std::queue<std::function<void()>> queued_event_processings_;
};

} //namespace

#endif
