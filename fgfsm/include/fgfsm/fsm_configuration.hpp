//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_FSM_CONFIGURATION_HPP
#define FGFSM_FSM_CONFIGURATION_HPP

#include "state_transition_helper.hpp"
#include "internal_transition_policy.hpp"

namespace fgfsm
{

struct fsm_configuration
{
    using internal_transition_policy = default_internal_transition_policy;

    template<class StateTransitionHelper>
    static void perform_state_transition(StateTransitionHelper& helper)
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

    static constexpr auto enable_event_queue = true;
};

} //namespace

#endif
