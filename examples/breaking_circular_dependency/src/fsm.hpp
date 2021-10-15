//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FSM_HPP
#define FSM_HPP

#include "fsm_fwd.hpp"
#include "events.hpp"
#include "context.hpp"
#include "states/idle.hpp"
#include "states/initializing.hpp"
#include "states/running.hpp"
#include <fgfsm.hpp>

using transition_table = fgfsm::transition_table
<
    fgfsm::row<states::idle,         events::start,                 states::initializing>,
    fgfsm::row<states::initializing, events::end_of_initialization, states::running>
>;

using fsm_impl = fgfsm::fsm<transition_table>;

class fsm
{
    public:
        fsm():
            ctx_{*this},
            impl_(ctx_)
        {
        }

        template<class Event>
        void process_event(const Event& event)
        {
            impl_.process_event(event);
        }

    private:
        context ctx_;
        fsm_impl impl_;
};

#define X(EVENT) \
    void process_event(fsm& sm, const EVENT& event) \
    { \
        sm.process_event(event); \
    }

STATE_EVENTS

#undef X

#endif
