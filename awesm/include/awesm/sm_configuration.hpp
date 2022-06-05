//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SM_CONFIGURATION_HPP
#define AWESM_SM_CONFIGURATION_HPP

#include "detail/default_exception_handler.hpp"
#include "detail/default_pre_transition_event_handler.hpp"
#include "detail/default_state_transition_hook_set.hpp"
#include <exception>

namespace awesm
{

struct sm_configuration
{
    using exception_handler = detail::default_exception_handler;
    using pre_transition_event_handler = detail::default_pre_transition_event_handler;
    using state_transition_hook_set = detail::default_state_transition_hook_set;
    static constexpr auto enable_run_to_completion = true;
    static constexpr auto enable_in_state_internal_transitions = true;
};

} //namespace

#endif
