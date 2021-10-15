//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include "fsm.hpp"

int main()
{
    auto sm = fsm{};
    sm.process_event(events::start{});

    return 0;
}
