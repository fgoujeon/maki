//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_STATE_TRANSITION_POLICY_HPP
#define FGFSM_STATE_TRANSITION_POLICY_HPP

#include "detail/call_state_member.hpp"
#include <type_traits>

namespace fgfsm
{

template
<
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

        state_transition_policy_helper
        (
            StartState& start_state,
            const Event& event,
            TargetState& target_state,
            Action& action,
            Guard& guard,
            int& active_state_index,
            bool& processed,
            const int target_state_index
        ):
            start_state_(start_state),
            event_(event),
            target_state_(target_state),
            action_(action),
            guard_(guard),
            active_state_index_(active_state_index),
            processed_(processed),
            target_state_index_(target_state_index)
        {
        }

    public:
        bool check_guard() const
        {
            return guard_(start_state_, event_, target_state_);
        }

        void invoke_start_state_on_exit()
        {
            detail::call_on_exit(start_state_, event_);
        }

        void activate_target_state()
        {
            active_state_index_ = target_state_index_;
            processed_ = true;
        }

        void execute_action()
        {
            action_(start_state_, event_, target_state_);
        }

        void invoke_target_state_on_entry()
        {
            detail::call_on_entry(target_state_, event_);
        }

    private:
        StartState& start_state_;
        const Event& event_;
        TargetState& target_state_;
        Action& action_;
        Guard& guard_;
        int& active_state_index_;
        bool& processed_;
        const int target_state_index_;
};

struct fast_state_transition_policy
{
    template<class... Args>
    fast_state_transition_policy(const Args&...)
    {
    }

    template
    <
        class StartState,
        class Event,
        class TargetState,
        class Action,
        class Guard
    >
    void operator()
    (
        state_transition_policy_helper<StartState, Event, TargetState, Action, Guard>& helper
    )
    {
        if(helper.check_guard())
        {
            helper.invoke_start_state_on_exit();
            helper.activate_target_state();
            helper.execute_action();
            helper.invoke_target_state_on_entry();
        }
    }
};

} //namespace

#endif
