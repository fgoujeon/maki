//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_without_event.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_DETAIL_CALL_STATE_MEMBER_HPP
#define FGFSM_DETAIL_CALL_STATE_MEMBER_HPP

#include "ignore_unused.hpp"
#include "../event.hpp"
#include <utility>

namespace fgfsm::detail
{

/*
call_on_xxx(state, event) calls either (in this order of priority):
- state.on_xxx(event);
- state.on_xxx() if the above function doesn't exist;
- nothing, if the above functions don't exist.
*/

#define FGFSM_DETAIL_HAS_MEMBER_FUNCTION(MEMBER_NAME) \
    template<class T, class... Args> \
    constexpr auto has_##MEMBER_NAME##_impl(int) -> decltype(std::declval<T>().MEMBER_NAME(std::declval<Args>()...), bool()) \
    { \
        return true; \
    } \
 \
    template<class T, class... Args> \
    constexpr bool has_##MEMBER_NAME##_impl(long) \
    { \
        return false; \
    } \
 \
    template<class T, class... Args> \
    constexpr bool has_##MEMBER_NAME() \
    { \
        return has_##MEMBER_NAME##_impl<T, Args...>(0); \
    }

FGFSM_DETAIL_HAS_MEMBER_FUNCTION(on_entry)
FGFSM_DETAIL_HAS_MEMBER_FUNCTION(on_event)
FGFSM_DETAIL_HAS_MEMBER_FUNCTION(on_exit)

#undef FGFSM_DETAIL_CALL_STATE_MEMBER

template<class State>
void call_on_entry(State& state, const event_ref& evt)
{
    if constexpr(has_on_entry<State&, const event_ref&>())
        state.on_entry(evt);
    else if constexpr(has_on_entry<State&>())
        state.on_entry();
    ignore_unused(state, evt);
}

template<class State>
void call_on_event(State& state, const event_ref& evt)
{
    if constexpr(has_on_event<State&, const event_ref&>())
        state.on_event(evt);
    ignore_unused(state, evt);
}

template<class State>
void call_on_exit(State& state, const event_ref& evt)
{
    if constexpr(has_on_exit<State&, const event_ref&>())
        state.on_exit(evt);
    else if constexpr(has_on_exit<State&>())
        state.on_exit();
    ignore_unused(state, evt);
}

} //namespace

#endif
