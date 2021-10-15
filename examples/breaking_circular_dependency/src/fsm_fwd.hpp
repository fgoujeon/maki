//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FSM_FWD_HPP
#define FSM_FWD_HPP

#include "events.hpp"

class fsm;

//List of events that we want states to be able to send
#define STATE_EVENTS \
    X(events::end_of_initialization)

#define X(EVENT) \
    void process_event(fsm& sm, const EVENT& event);

STATE_EVENTS

#undef X

#endif
