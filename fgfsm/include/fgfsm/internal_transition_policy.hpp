//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_INTERNAL_TRANSITION_POLICY_HPP
#define FGFSM_INTERNAL_TRANSITION_POLICY_HPP

#include "any_copy.hpp"
#include <type_traits>

namespace fgfsm
{

template<class State>
class internal_transition_policy_helper
{
    private:
        template<class TransitionTable, class Configuration>
        friend class fsm;

        internal_transition_policy_helper
        (
            State& state,
            const any_cref& event
        ):
            state_(state),
            evt_(event)
        {
        }

    public:
        void invoke_state_on_event()
        {
            state_.on_event(evt_);
        }

    private:
        State& state_;
        const any_cref& evt_;
};

struct default_internal_transition_policy
{
    template<class Context>
    default_internal_transition_policy(const Context&)
    {
    }

    template<class State>
    void operator()(internal_transition_policy_helper<State>& helper)
    {
        helper.invoke_state_on_event();
    }
};

} //namespace

#endif
