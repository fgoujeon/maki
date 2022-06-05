//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SIMPLE_SM_CONFIGURATION_HPP
#define AWESM_SIMPLE_SM_CONFIGURATION_HPP

#include "detail/default_exception_handler.hpp"
#include "detail/default_state_transition_hook_set.hpp"
#include <exception>

namespace awesm
{

struct simple_sm_configuration
{
    using exception_handler = detail::default_exception_handler;

    struct pre_transition_event_handler
    {
        template<class Sm, class Context>
        pre_transition_event_handler(Sm& /*machine*/, Context& /*ctx*/)
        {
        }

        /*
        Called whenever an event is being processed, after recursive call
        protection and just before performing internal and state
        transitions.
        It is useful for handling event independently of the active state by
        taking advantage of the run-to-completion mechanism.
        */
        //void on_event(const Event&);
    };

    using state_transition_hook_set = detail::default_state_transition_hook_set;

    static constexpr auto enable_run_to_completion = true;
    static constexpr auto enable_in_state_internal_transitions = true;
};

} //namespace

#endif
