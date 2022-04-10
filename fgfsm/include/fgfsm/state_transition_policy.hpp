//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_STATE_TRANSITION_POLICY_HPP
#define FGFSM_STATE_TRANSITION_POLICY_HPP

#include "any_copy.hpp"
#include "none.hpp"
#include "detail/call_state_member.hpp"
#include <type_traits>
#include <cassert>

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
            TargetState* const ptarget_state,
            Action* const paction,
            Guard* const pguard,
            int& active_state_index,
            bool& processed,
            const int target_state_index
        ):
            start_state_(start_state),
            evt_(event),
            ptarget_state_(ptarget_state),
            paction_(paction),
            pguard_(pguard),
            active_state_index_(active_state_index),
            processed_(processed),
            target_state_index_(target_state_index)
        {
        }

    public:
        bool check_guard()
        {
            if constexpr(!std::is_same_v<Guard, none>)
            {
                assert(pguard_);
                return pguard_->check(evt_);
            }
            else
            {
                return true;
            }
        }

        void validate_transition()
        {
            processed_ = true;
        }

        void invoke_start_state_on_exit()
        {
            [&](auto /*dummy*/)
            {
                if constexpr(!std::is_same_v<TargetState, none>)
                    detail::call_on_exit(start_state_, evt_);
            }(0);
        }

        void activate_target_state()
        {
            [&](auto /*dummy*/)
            {
                if constexpr(!std::is_same_v<TargetState, none>)
                    active_state_index_ = target_state_index_;
            }(0);
        }

        void execute_action()
        {
            [&](auto /*dummy*/)
            {
                if constexpr(!std::is_same_v<Action, none>)
                {
                    assert(paction_);
                    paction_->execute(evt_);
                }
            }(0);
        }

        void invoke_target_state_on_entry()
        {
            [&](auto /*dummy*/)
            {
                if constexpr(!std::is_same_v<TargetState, none>)
                {
                    assert(ptarget_state_);
                    detail::call_on_entry(*ptarget_state_, evt_);
                }
            }(0);
        }

    private:
        StartState& start_state_;
        const Event& evt_;
        TargetState* const ptarget_state_ = nullptr;
        Action* const paction_ = nullptr;
        Guard* const pguard_ = nullptr;
        int& active_state_index_;
        bool& processed_;
        const int target_state_index_;
};

struct default_state_transition_policy
{
    template<class Context>
    default_state_transition_policy(const Context&)
    {
    }

    template<class Helper>
    void do_transition(Helper& helper)
    {
        if(helper.check_guard())
        {
            helper.validate_transition();
            helper.invoke_start_state_on_exit();
            helper.activate_target_state();
            helper.execute_action();
            helper.invoke_target_state_on_entry();
        }
    }
};

} //namespace

#endif
