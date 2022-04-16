//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_STATE_TRANSITION_POLICY_HELPER_HPP
#define FGFSM_STATE_TRANSITION_POLICY_HELPER_HPP

#include "none.hpp"
#include "detail/tlu.hpp"
#include "detail/tuple.hpp"
#include "detail/call_member.hpp"
#include <type_traits>
#include <cassert>

namespace fgfsm
{

template
<
    class Fsm,
    class StartState,
    class Event,
    class TargetState,
    class Action,
    class Guard
>
class state_transition_policy_helper
{
    private:
        template<class TransitionTable, class Configuration>
        friend class fsm;

        state_transition_policy_helper(Fsm& sm, const Event& event):
            sm_(sm),
            event_(event)
        {
        }

    public:
        bool check_guard() const
        {
            if constexpr(!std::is_same_v<Guard, none>)
            {
                return detail::call_check
                (
                    detail::get<Guard>(sm_.guards_),
                    event_
                );
            }
            else
            {
                return true;
            }
        }

        void invoke_start_state_on_exit() const
        {
            [&](auto)
            {
                if constexpr(!std::is_same_v<TargetState, none>)
                {
                    detail::call_on_exit
                    (
                        detail::get<StartState>(sm_.states_),
                        event_
                    );
                }
            }(0);
        }

        void activate_target_state() const
        {
            [&](auto)
            {
                if constexpr(!std::is_same_v<TargetState, none>)
                {
                    sm_.active_state_index_ = detail::tlu::get_index
                    <
                        typename Fsm::state_tuple,
                        TargetState
                    >;
                }
            }(0);
        }

        void execute_action() const
        {
            [&](auto)
            {
                if constexpr(!std::is_same_v<Action, none>)
                {
                    detail::call_execute
                    (
                        detail::get<Action>(sm_.actions_),
                        event_
                    );
                }
            }(0);
        }

        void invoke_target_state_on_entry() const
        {
            [&](auto)
            {
                if constexpr(!std::is_same_v<TargetState, none>)
                {
                    detail::call_on_entry
                    (
                        detail::get<TargetState>(sm_.states_),
                        event_
                    );
                }
            }(0);
        }

    private:
        Fsm& sm_;
        const Event& event_;
};

} //namespace

#endif
