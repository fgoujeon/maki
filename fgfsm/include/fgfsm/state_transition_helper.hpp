//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_STATE_TRANSITION_HELPER_HPP
#define FGFSM_STATE_TRANSITION_HELPER_HPP

#include "any_copy.hpp"
#include "none.hpp"
#include <type_traits>
#include <cassert>

namespace fgfsm
{

template
<
    class Context,
    class StartState,
    class TargetState,
    class Action,
    class Guard
>
class state_transition_helper
{
    private:
        template<class Configuration>
        friend class fsm;

        state_transition_helper
        (
            Context& ctx,
            StartState& start_state,
            const any_cref& event,
            TargetState* const ptarget_state,
            Action& action,
            Guard& guard,
            int& active_state_index,
            bool& processed,
            const int target_state_index
        ):
            ctx_(ctx),
            start_state_(start_state),
            evt_(event),
            ptarget_state_(ptarget_state),
            action_(action),
            guard_(guard),
            active_state_index_(active_state_index),
            processed_(processed),
            target_state_index_(target_state_index)
        {
        }

    public:
        bool check_guard()
        {
            return guard_(ctx_, evt_);
        }

        void validate_transition()
        {
            processed_ = true;
        }

        void invoke_start_state_on_exit()
        {
            if constexpr(!std::is_same_v<TargetState, none>)
                start_state_.on_exit(evt_);
        }

        void activate_target_state()
        {
            if constexpr(!std::is_same_v<TargetState, none>)
                active_state_index_ = target_state_index_;
        }

        void execute_action()
        {
            action_(ctx_, evt_);
        }

        void invoke_target_state_on_entry()
        {
            if constexpr(!std::is_same_v<TargetState, none>)
            {
                assert(ptarget_state_);
                ptarget_state_->on_entry(evt_);
            }
        }

    private:
        Context& ctx_;
        StartState& start_state_;
        const any_cref& evt_;
        TargetState* const ptarget_state_ = nullptr;
        Action& action_;
        Guard& guard_;
        int& active_state_index_;
        bool& processed_;
        const int target_state_index_;
};

} //namespace

#endif
