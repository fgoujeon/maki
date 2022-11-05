//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_ROW_HPP
#define AWESM_ROW_HPP

#include "null.hpp"

namespace awesm
{

template
<
    class SourceState,
    class Event,
    class TargetState,
    class Action = null,
    class Guard = null
>
struct row
{
    using source_state_type = SourceState;
    using event_type = Event;
    using target_state_type = TargetState;
    using action_type = Action;
    using guard_type = Guard;
};

} //namespace

#endif
