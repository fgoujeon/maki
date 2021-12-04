//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_INTERNAL_TRANSITION_POLICY_HPP
#define FGFSM_INTERNAL_TRANSITION_POLICY_HPP

#include "event.hpp"
#include "detail/call_state_member.hpp"
#include <type_traits>

//Invocation policy of active_state.on_event(event)

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
            const event_ref& evt
        ):
            state_(state),
            evt_(evt)
        {
        }

    public:
        void invoke_state_on_event()
        {
            detail::call_on_event(state_, evt_);
        }

    private:
        State& state_;
        const event_ref& evt_;
};

struct fast_internal_transition_policy
{
    template<class Context>
    fast_internal_transition_policy(const Context&)
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
