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
FGFSM_DETAIL_HAS_MEMBER_FUNCTION(execute)
FGFSM_DETAIL_HAS_MEMBER_FUNCTION(check)

#undef FGFSM_DETAIL_CALL_STATE_MEMBER

template<class State, class Event>
void call_on_entry(State& state, const Event& event)
{
    if constexpr(has_on_entry<State&, const Event&>())
    {
        static_assert(!std::is_empty_v<State>, "Empty state types can't have on_entry() function");
        state.on_entry(event);
    }
    else if constexpr(has_on_entry<State&>())
    {
        static_assert(!std::is_empty_v<State>, "Empty state types can't have on_entry() function");
        ignore_unused(event);
        state.on_entry();
    }
    else if constexpr(!std::is_empty_v<State>)
        int* error = "No on_entry(event) or on_entry() found in non-empty state type";
    else
        ignore_unused(event);
}

template<class State, class Event>
void call_on_event(State& state, const Event& event)
{
    if constexpr(has_on_event<State&, const Event&>())
        state.on_event(event);
    else
        ignore_unused(state, event);
}

template<class State, class Event>
void call_on_exit(State& state, const Event& event)
{
    if constexpr(has_on_exit<State&, const Event&>())
    {
        static_assert(!std::is_empty_v<State>, "Empty state types can't have on_exit() function");
        state.on_exit(event);
    }
    else if constexpr(has_on_exit<State&>())
    {
        static_assert(!std::is_empty_v<State>, "Empty state types can't have on_exit() function");
        ignore_unused(event);
        state.on_exit();
    }
    else if constexpr(!std::is_empty_v<State>)
        int* error = "No on_exit(event) or on_exit() found in non-empty state type";
    else
        ignore_unused(event);
}

template<class Action, class Event>
void call_execute(Action& action, const Event& event)
{
    if constexpr(has_execute<Action&, const Event&>())
        action.execute(event);
    else if constexpr(has_execute<Action&>())
    {
        ignore_unused(event);
        action.execute();
    }
    else
        int* error = "No execute(event) or execute() found in action type";
}

template<class Guard, class Event>
bool call_check(Guard& guard, const Event& event)
{
    if constexpr(has_check<Guard&, const Event&>())
        return guard.check(event);
    else if constexpr(has_check<Guard&>())
    {
        ignore_unused(event);
        return guard.check();
    }
    else
        int* error = "No check(event) or check() found in guard type";
}

} //namespace

#endif
