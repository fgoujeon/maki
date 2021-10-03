//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_without_event.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_DETAIL_CALL_STATE_MEMBER_HPP
#define FGFSM_DETAIL_CALL_STATE_MEMBER_HPP

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

template<class State, class Event>
void call_on_entry(State& state, const Event& event)
{
    if constexpr(has_on_entry<State&, const Event&>())
        state.on_entry(event);
    else if constexpr(has_on_entry<State&>())
        state.on_entry();
    else
        int* error = "No state::on_entry(event) or state::on_entry() found";
}

template<class State, class Event>
void call_on_event(State& state, const Event& event)
{
    if constexpr(has_on_event<State&, const Event&>())
        state.on_event(event);
}

template<class State, class Event>
void call_on_exit(State& state, const Event& event)
{
    if constexpr(has_on_exit<State&, const Event&>())
        state.on_exit(event);
    else if constexpr(has_on_exit<State&>())
        state.on_exit();
    else
        int* error = "No state::on_exit(event) or state::on_exit() found";
}

} //namespace

#endif
