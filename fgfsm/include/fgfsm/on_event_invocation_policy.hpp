//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_ON_EVENT_INVOCATION_POLICY_HPP
#define FGFSM_ON_EVENT_INVOCATION_POLICY_HPP

#include "detail/call_state_member.hpp"
#include <type_traits>

//Invocation policy of active_state.on_event(event)

namespace fgfsm
{

template<class State, class Event>
class on_event_invocation_policy_helper
{
    private:
        template
        <
            class TransitionTable,
            class TransitionPolicy,
            class OnEventInvocationPolicy
        >
        friend class fsm;

        on_event_invocation_policy_helper
        (
            State& state,
            const Event& event
        ):
            state_(state),
            event_(event)
        {
        }

    public:
        void invoke_state_on_event()
        {
            detail::call_on_event(state_, event_);
        }

    private:
        State& state_;
        const Event& event_;
};

struct fast_on_event_invocation_policy
{
    template<class... Args>
    fast_on_event_invocation_policy(const Args&...)
    {
    }

    template<class State, class Event>
    void operator()(on_event_invocation_policy_helper<State, Event>& helper)
    {
        helper.invoke_state_on_event();
    }
};

} //namespace

#endif
