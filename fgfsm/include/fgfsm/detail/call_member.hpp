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
    {
        ignore_unused(event);
        static_assert
        (
            std::is_empty_v<State>,
            "No on_entry(event) or on_entry() found in non-empty state type"
        );
    }
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
    else
    {
        ignore_unused(event);
        static_assert
        (
            std::is_empty_v<State>,
            "No on_exit(event) or on_exit() found in non-empty state type"
        );
    }
}

template<class Action, class StartState, class Event, class TargetState>
void call_execute
(
    Action& action,
    StartState& start_state,
    const Event& event,
    TargetState& target_state
)
{
    static_assert
    (
        has_execute<Action&, StartState&, const Event&, TargetState&>() ||
        has_execute<Action&, const Event&>() ||
        has_execute<Action&>(),
        "No execute(start_state, event, target_state), "
        "execute(event) or execute() found in action type"
    );

    if constexpr(has_execute<Action&, StartState&, const Event&, TargetState&>())
    {
        action.execute(start_state, event, target_state);
    }
    else if constexpr(has_execute<Action&, const Event&>())
    {
        ignore_unused(start_state, target_state);
        action.execute(event);
    }
    else if constexpr(has_execute<Action&>())
    {
        ignore_unused(start_state, event, target_state);
        action.execute();
    }
}

template<class Guard, class StartState, class Event, class TargetState>
bool call_check
(
    Guard& guard,
    StartState& start_state,
    const Event& event,
    TargetState& target_state
)
{
    static_assert
    (
        has_check<Guard&, StartState&, const Event&, TargetState&>() ||
        has_check<Guard&, const Event&>() ||
        has_check<Guard&>(),
        "No check(start_state, event, target_state), "
        "check(event) or check() found in guard type"
    );

    if constexpr(has_check<Guard&, StartState&, const Event&, TargetState&>())
    {
        return guard.check(start_state, event, target_state);
    }
    else if constexpr(has_check<Guard&, const Event&>())
    {
        ignore_unused(start_state, target_state);
        return guard.check(event);
    }
    else if constexpr(has_check<Guard&>())
    {
        ignore_unused(start_state, event, target_state);
        return guard.check();
    }
}

} //namespace

#endif
