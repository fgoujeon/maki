//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_ROW_HPP
#define FGFSM_ROW_HPP

#include "none.hpp"
#include "any_cref.hpp"

namespace fgfsm
{

struct null_action
{
    template<class Context>
    void operator()(Context&, const any_cref&)
    {
    }
};

struct null_guard
{
    template<class Context>
    bool operator()(Context&, const any_cref&)
    {
        return true;
    }
};

template
<
    class StartState,
    class Event,
    class TargetState,
    class Action = null_action,
    class Guard = null_guard
>
struct row
{
    using start_state_t = StartState;
    using event_t = Event;
    using target_state_t = TargetState;
    using action_t = Action;
    using guard_t = Guard;

    action_t action;
    guard_t guard;
};

template
<
    class StartState,
    class Event,
    class TargetState = none,
    class ActionArg = null_action,
    class GuardArg = null_guard
>
auto make_row
(
    ActionArg&& action = null_action{},
    GuardArg&& guard = null_guard{}
)
{
    using action_t = std::decay_t<ActionArg>;
    using guard_t = std::decay_t<GuardArg>;
    return row<StartState, Event, TargetState, action_t, guard_t>
    {
        std::forward<ActionArg>(action),
        std::forward<GuardArg>(guard)
    };
}

} //namespace

#endif
