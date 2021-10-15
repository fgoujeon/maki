//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef STATES_IDLE_HPP
#define STATES_IDLE_HPP

#include "../context.hpp"

namespace states
{

struct idle
{
    void on_entry(){}

    void on_exit(){}

    context& ctx;
};

} //namespace

#endif
