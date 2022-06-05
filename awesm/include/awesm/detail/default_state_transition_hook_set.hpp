//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_DEFAULT_STATE_TRANSITION_HOOK_SET_HPP
#define AWESM_DETAIL_DEFAULT_STATE_TRANSITION_HOOK_SET_HPP

namespace awesm::detail
{

struct default_state_transition_hook_set
{
    template<class Sm, class Context>
    default_state_transition_hook_set(Sm& /*machine*/, Context& /*ctx*/)
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

} //namespace

#endif
