//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_FSM_HPP
#define FGFSM_FSM_HPP

#include "detail/call_state_member.hpp"
#include "detail/for_each.hpp"
#include "detail/make_tuple.hpp"
#include "detail/transition_table_digest.hpp"

namespace fgfsm
{

template<class TransitionTable>
class fsm
{
    public:
        using transition_table = TransitionTable;
        using transition_table_digest =
            detail::transition_table_digest<transition_table>
        ;

        using state_tuple  = typename transition_table_digest::state_tuple;
        using action_tuple = typename transition_table_digest::action_tuple;
        using guard_tuple  = typename transition_table_digest::guard_tuple;

    public:
        template<typename... Args>
        fsm(Args&&... args):
            states_(detail::make_tuple<state_tuple>(args...)),
            actions_(detail::make_tuple<action_tuple>(args...)),
            guards_(detail::make_tuple<guard_tuple>(args...))
        {
        }

        //Check whether the given State is the current state type
        template<class State>
        bool is_current_state() const
        {
            constexpr auto given_state_index = detail::tlu::get_index
            <
                state_tuple,
                State
            >;
            return given_state_index == current_state_index_;
        }

        template<class Event>
        void process_event(const Event& event)
        {
            const auto processed = process_event_in_transition_table(event);
            if(!processed)
                process_event_in_current_state(event);
        }

    private:
        //Try and trigger a transition
        template<class Event>
        bool process_event_in_transition_table(const Event& event)
        {
            bool processed = false;

            detail::tlu::for_each<transition_table> //For each row
            (
                [this, &event, &processed](auto* ptransition)
                {
                    using transition              = std::remove_pointer_t<decltype(ptransition)>;
                    using transition_start_state  = typename transition::start_state;
                    using transition_event        = typename transition::event;
                    using transition_target_state = typename transition::target_state;
                    using transition_action       = typename transition::action;
                    using transition_guard        = typename transition::guard;

                    if constexpr(std::is_same_v<transition_event, Event>)
                    {
                        //Make sure we don't trigger more than one transition
                        if(processed)
                            return;

                        //Make sure the transition start state is the current
                        //state
                        if(!is_current_state<transition_start_state>())
                            return;

                        auto& start_state =
                            std::get<transition_start_state>(states_)
                        ;
                        auto& target_state =
                            std::get<transition_target_state>(states_)
                        ;
                        auto& guard = std::get<transition_guard>(guards_);

                        //Make sure the guard authorizes the transition
                        if(!guard(start_state, event, target_state))
                            return;

                        auto& action = std::get<transition_action>(actions_);

                        //Perform the transition
                        {
                            detail::call_on_exit(start_state, event);

                            current_state_index_ = detail::tlu::get_index
                            <
                                state_tuple,
                                transition_target_state
                            >;

                            action(start_state, event, target_state);

                            detail::call_on_entry(target_state, event);
                        }

                        processed = true;
                    }
                }
            );

            return processed;
        }

        /*
        Call current_state.on_event(event)
        */
        template<class Event>
        void process_event_in_current_state(const Event& event)
        {
            detail::for_each
            (
                states_,
                [this, &event](auto& s)
                {
                    using state = std::decay_t<decltype(s)>;
                    if(is_current_state<state>())
                    {
                        detail::call_on_event(s, event);
                    }
                }
            );
        }

    private:
        state_tuple states_;
        action_tuple actions_;
        guard_tuple guards_;
        int current_state_index_ = 0;
};

} //namespace

#endif
