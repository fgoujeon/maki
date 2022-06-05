//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SUBSM_CONFIGURATION_HPP
#define AWESM_SUBSM_CONFIGURATION_HPP

#include "detail/default_exception_handler.hpp"

namespace awesm
{

struct subsm_configuration
{
    using exception_handler = detail::default_exception_handler;

    struct state_transition_hook_set
    {
        template<class Sm, class Context>
        state_transition_hook_set(Sm& /*machine*/, Context& /*ctx*/)
        {
        }

        template<class SourceState, class Event, class TargetState>
        void before_transition(const Event& /*event*/)
        {
        }

        template<class SourceState, class Event, class TargetState>
        void after_transition(const Event& /*event*/)
        {
        }
    };

    static constexpr auto enable_in_state_internal_transitions = true;
};

} //namespace

#endif
