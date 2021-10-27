//Copyright Florian Goujeon 2021.
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
    using state_transition_policy = fast_state_transition_policy;
    using internal_transition_policy = fast_internal_transition_policy;
};

} //namespace

#endif
