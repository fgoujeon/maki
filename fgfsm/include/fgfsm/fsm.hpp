//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_FSM_HPP
#define FGFSM_FSM_HPP

#include "transition_policy.hpp"
#include "on_event_invocation_policy.hpp"
#include "none.hpp"
#include "event_ref.hpp"
#include "detail/call_state_member.hpp"
#include "detail/for_each.hpp"
#include "detail/make_tuple.hpp"
#include "detail/transition_table_digest.hpp"
#include <queue>

namespace fgfsm
{

template
<
    class TransitionTable,
    class TransitionPolicy = fast_transition_policy,
    class OnEventInvocationPolicy = fast_on_event_invocation_policy
>
class fsm
{
    private:
        using transition_table = TransitionTable;
        using transition_table_digest =
            detail::transition_table_digest<transition_table>
        ;

        using state_tuple  = typename transition_table_digest::state_tuple;
        using action_tuple = typename transition_table_digest::action_tuple;
        using guard_tuple  = typename transition_table_digest::guard_tuple;

    public:
        template<class Context>
        fsm(Context& context):
            states_(detail::make_tuple<state_tuple>(context)),
            actions_(detail::make_tuple<action_tuple>(context)),
            guards_(detail::make_tuple<guard_tuple>(context)),
            transition_policy_(context),
            on_event_invocation_policy_(context)
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

        void process_event(const event_ref& evt)
        {
            //Defer event processing in case of recursive call
            if(processing_event_)
            {
                deferred_events_.push(evt);
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

            process_event_once(evt);

            //Process deferred event processings
            while(!deferred_events_.empty())
            {
                process_event_once(deferred_events_.front());
                deferred_events_.pop();
            }
        }

    private:
        void process_event_once(const event_ref& evt)
        {
            const auto processed = process_event_in_transition_table(evt);
            if(!processed)
                process_event_in_active_state(evt);
        }

        /*
        Try and trigger a transition and potential subsequent anonymous
        transitions, if any.
        */
        bool process_event_in_transition_table(const event_ref& evt)
        {
            const bool processed = process_event_in_transition_table_once(evt);

            //Anonymous transitions
            if(processed)
                while(process_event_in_transition_table_once(none{}));

            return processed;
        }

        //Try and trigger one transition
        bool process_event_in_transition_table_once(const event_ref& evt)
        {
            bool processed = false;

            detail::tlu::for_each<transition_table> //For each row
            (
                [this, &evt, &processed](auto* ptransition)
                {
                    using transition              = std::remove_pointer_t<decltype(ptransition)>;
                    using transition_start_state  = typename transition::start_state;
                    using transition_event        = typename transition::event;
                    using transition_target_state = typename transition::target_state;
                    using transition_action       = typename transition::action;
                    using transition_guard        = typename transition::guard;

                    //Make sure we don't trigger more than one transition
                    if(processed)
                        return;

                    if(evt.is<transition_event>())
                    {
                        //Make sure the transition start state is the active
                        //state
                        if(!is_active_state<transition_start_state>())
                            return;

                        auto& start_state =
                            std::get<transition_start_state>(states_)
                        ;
                        auto& target_state =
                            std::get<transition_target_state>(states_)
                        ;
                        auto& guard = std::get<transition_guard>(guards_);
                        auto& action = std::get<transition_action>(actions_);

                        const auto target_state_index = detail::tlu::get_index
                        <
                            state_tuple,
                            transition_target_state
                        >;

                        auto helper = transition_policy_helper
                        <
                            transition_start_state,
                            transition_target_state,
                            transition_action,
                            transition_guard
                        >
                        {
                            start_state,
                            evt,
                            target_state,
                            action,
                            guard,
                            active_state_index_,
                            processed,
                            target_state_index
                        };

                        //Perform the transition
                        transition_policy_(helper);
                    }
                }
            );

            return processed;
        }

        /*
        Call active_state.on_event(event)
        */
        void process_event_in_active_state(const event_ref& evt)
        {
            detail::for_each
            (
                states_,
                [this, &evt](auto& s)
                {
                    using state = std::decay_t<decltype(s)>;
                    if(is_active_state<state>())
                    {
                        auto helper = on_event_invocation_policy_helper
                        <
                            state
                        >{s, evt};
                        on_event_invocation_policy_(helper);
                    }
                }
            );
        }

    private:
        state_tuple states_;
        action_tuple actions_;
        guard_tuple guards_;
        TransitionPolicy transition_policy_;
        OnEventInvocationPolicy on_event_invocation_policy_;

        int active_state_index_ = 0;
        bool processing_event_ = false;
        std::queue<event_ref> deferred_events_;
};

} //namespace

#endif
