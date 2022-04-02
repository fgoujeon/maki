//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_FSM_CONFIGURATION_HPP
#define FGFSM_FSM_CONFIGURATION_HPP

#include "state_transition_policy.hpp"
#include "internal_transition_policy.hpp"

namespace fgfsm
{

struct fsm_configuration
{
    /*
    Called whenever an event is being processed, after recursive call protection
    and just before performing internal and state transitions.
    It is useful for handling event independently of the active state by taking
    advantage of the run-to-completion mechanism.
    */
    struct pre_transition_event_handler
    {
        template<class Context>
        pre_transition_event_handler(Context&)
        {
        }

        void on_event(const any_cref&)
        {
        }
    };

    using internal_transition_policy = default_internal_transition_policy;

    using state_transition_policy = default_state_transition_policy;

    static constexpr auto enable_run_to_completion = true;
    static constexpr auto enable_in_state_internal_transitions = true;
};

} //namespace

#endif
