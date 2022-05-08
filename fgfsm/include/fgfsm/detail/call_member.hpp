//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_without_event.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_DETAIL_CALL_MEMBER_HPP
#define FGFSM_DETAIL_CALL_MEMBER_HPP

#include "ignore_unused.hpp"
#include <type_traits>
#include <utility>

namespace fgfsm::detail
{

/*
A 0 must be passed to int and long dummy parameters.
We use this trick so that int overloads take precedence over long ones.
*/

template<class State, class Event>
auto call_on_entry(State* pstate, const Event* pevent, int /*dummy*/) -> decltype(pstate->on_entry(*pevent))
{
    static_assert(!std::is_empty_v<State>, "Empty state types can't have on_entry() functions");
    pstate->on_entry(*pevent);
}

template<class State>
auto call_on_entry(State* pstate, const void* /*pevent*/, int /*dummy*/) -> decltype(pstate->on_entry())
{
    static_assert(!std::is_empty_v<State>, "Empty state types can't have on_entry() functions");
    pstate->on_entry();
}

template<class State>
void call_on_entry(State* /*pstate*/, const void* /*pevent*/, long /*dummy*/)
{
    static_assert
    (
        std::is_empty_v<State>,
        "No on_entry(event) or on_entry() found in non-empty state type"
    );
}

template<class State, class Event>
auto call_on_event(State* pstate, const Event* pevent, int /*dummy*/) -> decltype(pstate->on_event(*pevent))
{
    pstate->on_event(*pevent);
}

inline void call_on_event(void* /*pstate*/, const void* /*pevent*/, long /*dummy*/)
{
    //state::on_event() is optional
}

template<class State, class Event>
auto call_on_exit(State* pstate, const Event* pevent, int /*dummy*/) -> decltype(pstate->on_exit(*pevent))
{
    static_assert(!std::is_empty_v<State>, "Empty state types can't have on_exit() functions");
    pstate->on_exit(*pevent);
}

template<class State>
auto call_on_exit(State* pstate, const void* /*pevent*/, int /*dummy*/) -> decltype(pstate->on_exit())
{
    static_assert(!std::is_empty_v<State>, "Empty state types can't have on_exit() functions");
    pstate->on_exit();
}

template<class State>
void call_on_exit(State* /*pstate*/, const void* /*pevent*/, long /*dummy*/)
{
    static_assert
    (
        std::is_empty_v<State>,
        "No on_exit(event) or on_exit() found in non-empty state type"
    );
}

template<class Action, class StartState, class Event, class TargetState>
auto call_execute
(
    Action* paction,
    StartState* pstart_state,
    const Event* pevent,
    TargetState* ptarget_state
) -> decltype(paction->execute(*pstart_state, *pevent, *ptarget_state))
{
    paction->execute(*pstart_state, *pevent, *ptarget_state);
}

template<class Action, class Event>
auto call_execute
(
    Action* paction,
    void* /*pstart_state*/,
    const Event* pevent,
    void* /*ptarget_state*/
) -> decltype(paction->execute(*pevent))
{
    paction->execute(*pevent);
}

template<class Action>
auto call_execute
(
    Action* paction,
    void* /*pstart_state*/,
    const void* /*pevent*/,
    void* /*ptarget_state*/
) -> decltype(paction->execute())
{
    paction->execute();
}

template<class Guard, class StartState, class Event, class TargetState>
auto call_check
(
    Guard* pguard,
    StartState* pstart_state,
    const Event* pevent,
    TargetState* ptarget_state
) -> decltype(pguard->check(*pstart_state, *pevent, *ptarget_state))
{
    return pguard->check(*pstart_state, *pevent, *ptarget_state);
}

template<class Guard, class Event>
auto call_check
(
    Guard* pguard,
    void* /*pstart_state*/,
    const Event* pevent,
    void* /*ptarget_state*/
) -> decltype(pguard->check(*pevent))
{
    return pguard->check(*pevent);
}

template<class Guard>
auto call_check
(
    Guard* pguard,
    void* /*pstart_state*/,
    const void* /*pevent*/,
    void* /*ptarget_state*/
) -> decltype(pguard->check())
{
    return pguard->check();
}

} //namespace

#endif
