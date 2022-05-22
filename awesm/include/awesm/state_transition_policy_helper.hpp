//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_STATE_TRANSITION_POLICY_HELPER_HPP
#define AWESM_STATE_TRANSITION_POLICY_HELPER_HPP

#include "none.hpp"
#include "detail/tlu.hpp"
#include "detail/call_member.hpp"
#include <type_traits>

namespace awesm
{

template
<
    class Sm,
    class SourceState,
    class Event,
    class TargetState,
    class Action,
    class Guard
>
class state_transition_policy_helper
{
    public:
        using source_state_type = SourceState;
        using event_type = Event;
        using target_state_type = TargetState;
        using action_type = Action;
        using guard_type = Guard;

        [[nodiscard]] bool check_guard()
        {
            if constexpr(!std::is_same_v<Guard, none>)
            {
                return detail::call_check
                (
                    &sm_.guards_.get(static_cast<Guard*>(nullptr)),
                    &sm_.states_.get(static_cast<SourceState*>(nullptr)),
                    &event_,
                    get_target_state_ptr()
                );
            }
            else
            {
                return true;
            }
        }

        void invoke_source_state_on_exit()
        {
            if constexpr(!std::is_same_v<TargetState, none>)
            {
                detail::call_on_exit
                (
                    &sm_.states_.get(static_cast<SourceState*>(nullptr)),
                    &event_,
                    0
                );
            }
        }

        void activate_target_state()
        {
            if constexpr(!std::is_same_v<TargetState, none>)
            {
                sm_.active_state_index_ = detail::tlu::get_index
                <
                    typename Sm::state_tuple_t,
                    TargetState
                >;
            }
        }

        void execute_action()
        {
            if constexpr(!std::is_same_v<Action, none>)
            {
                detail::call_execute
                (
                    &sm_.actions_.get(static_cast<Action*>(nullptr)),
                    &sm_.states_.get(static_cast<SourceState*>(nullptr)),
                    &event_,
                    get_target_state_ptr()
                );
            }
        }

        void invoke_target_state_on_entry()
        {
            if constexpr(!std::is_same_v<TargetState, none>)
            {
                detail::call_on_entry
                (
                    get_target_state_ptr(),
                    &event_,
                    0
                );
            }
        }

    private:
        template<class FsmConfiguration>
        friend class sm;

        state_transition_policy_helper(Sm& machine, const Event& event):
            sm_(machine),
            event_(event)
        {
        }

        auto get_target_state_ptr() const
        {
            if constexpr(std::is_same_v<TargetState, none>)
            {
                return nullptr;
            }
            else
            {
                return &sm_.states_.get(static_cast<TargetState*>(nullptr));
            }
        }

        Sm& sm_;
        const Event& event_;
};

} //namespace

#endif
