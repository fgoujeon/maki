//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "fsm_fwd.hpp"

struct context
{
    template<class Event>
    void process_event(const Event& event)
    {
        ::process_event(sm, event);
    }

    fsm& sm;
};

#endif
