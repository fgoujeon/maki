//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_without_event.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_CALL_MEMBER_HPP
#define AWESM_DETAIL_CALL_MEMBER_HPP

#include "ignore_unused.hpp"
#include <type_traits>
#include <utility>

namespace awesm::detail
{

/*
A 0 must be passed to int and long dummy parameters.
We use this trick so that int overloads take precedence over long ones.
*/

template<class State, class Sm, class Event>
auto call_on_entry(State* pstate, Sm* pmach, const Event* pevent, int /*dummy*/) ->
    decltype(pstate->on_entry(*pmach, *pevent))
{
    static_assert
    (
        !std::is_empty_v<State>,
        "Empty state types can't have on_entry() functions"
    );
    pstate->on_entry(*pmach, *pevent);
}

template<class State, class Event>
auto call_on_entry(State* pstate, void* /*pmach*/, const Event* pevent, int /*dummy*/) ->
    decltype(pstate->on_entry(*pevent))
{
    static_assert
    (
        !std::is_empty_v<State>,
        "Empty state types can't have on_entry() functions"
    );
    pstate->on_entry(*pevent);
}

template<class State>
auto call_on_entry(State* pstate, void* /*pmach*/, const void* /*pevent*/, int /*dummy*/) ->
    decltype(pstate->on_entry())
{
    static_assert
    (
        !std::is_empty_v<State>,
        "Empty state types can't have on_entry() functions"
    );
    pstate->on_entry();
}

template<class State>
void call_on_entry(State* /*pstate*/, void* /*pmach*/, const void* /*pevent*/, long /*dummy*/)
{
    static_assert
    (
        std::is_empty_v<State>,
        "No on_entry(event) or on_entry() found in non-empty state type"
    );
}

template<class State, class Sm, class Event>
auto call_on_event(State* pstate, Sm* pmach, const Event* pevent) ->
    decltype(pstate->on_event(*pevent))
{
    pstate->on_event(*pmach, *pevent);
}

template<class State, class Event>
auto call_on_event(State* pstate, void* /*pmach*/, const Event* pevent) ->
    decltype(pstate->on_event(*pevent))
{
    pstate->on_event(*pevent);
}

inline void call_on_event(void* /*pstate*/, void* /*pmach*/, const void* /*pevent*/)
{
    //state::on_event() is optional
}

template<class State, class SmConfiguration, class Event>
auto call_on_exit(State* pstate, SmConfiguration* pmach, const Event* pevent, int /*dummy*/) ->
    decltype(pstate->on_exit(*pmach, *pevent))
{
    static_assert
    (
        !std::is_empty_v<State>,
        "Empty state types can't have on_exit() functions"
    );
    pstate->on_exit(*pmach, *pevent);
}

template<class State, class Event>
auto call_on_exit(State* pstate, void* /*pmach*/, const Event* pevent, int /*dummy*/) ->
    decltype(pstate->on_exit(*pevent))
{
    static_assert
    (
        !std::is_empty_v<State>,
        "Empty state types can't have on_exit() functions"
    );
    pstate->on_exit(*pevent);
}

template<class State>
auto call_on_exit(State* pstate, void* /*pmach*/, const void* /*pevent*/, int /*dummy*/) ->
    decltype(pstate->on_exit())
{
    static_assert
    (
        !std::is_empty_v<State>,
        "Empty state types can't have on_exit() functions"
    );
    pstate->on_exit();
}

template<class State>
void call_on_exit(State* /*pstate*/, void* /*pmach*/, const void* /*pevent*/, long /*dummy*/)
{
    static_assert
    (
        std::is_empty_v<State>,
        "No on_exit(event) or on_exit() found in non-empty state type"
    );
}

template<class Action, class Event>
auto call_execute(Action* paction, const Event* pevent) ->
    decltype(paction->execute(*pevent))
{
    paction->execute(*pevent);
}

template<class Action>
auto call_execute(Action* paction, const void* /*pevent*/) ->
    decltype(paction->execute())
{
    paction->execute();
}

template<class Guard, class Event>
auto call_check(Guard* pguard, const Event* pevent) ->
    decltype(pguard->check(*pevent))
{
    return pguard->check(*pevent);
}

template<class Guard>
auto call_check(Guard* pguard, const void* /*pevent*/) ->
    decltype(pguard->check())
{
    return pguard->check();
}

} //namespace

#endif
