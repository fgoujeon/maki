//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_INTERNAL_TRANSITION_POLICY_HELPER_HPP
#define FGFSM_INTERNAL_TRANSITION_POLICY_HELPER_HPP

#include "detail/call_member.hpp"

namespace fgfsm
{

template<class State, class Event>
class internal_transition_policy_helper
{
    private:
        template<class TransitionTable, class Configuration>
        friend class fsm;

        internal_transition_policy_helper(State& state, const Event& event):
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

} //namespace

#endif
