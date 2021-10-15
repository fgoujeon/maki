//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include "initializing.hpp"
#include "../events.hpp"
#include "../fsm_fwd.hpp"
#include <iostream>

namespace states
{

void initializing::on_entry()
{
    std::cout << "Initializing...\n";
    ctx.process_event(events::end_of_initialization{});
}

void initializing::on_exit()
{
    std::cout << "Initialized.\n";
}

} //namespace
