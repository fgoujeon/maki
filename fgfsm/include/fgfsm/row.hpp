//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_ROW_HPP
#define FGFSM_ROW_HPP

#include "none.hpp"

namespace fgfsm
{

template
<
    class StartState,
    class Event,
    class TargetState,
    class Action = none,
    class Guard = none
>
struct row
{
    using start_state_type = StartState;
    using event_type = Event;
    using target_state_type = TargetState;
    using action_type = Action;
    using guard_type = Guard;
};

} //namespace

#endif
