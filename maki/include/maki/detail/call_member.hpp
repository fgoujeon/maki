//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_without_event.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_CALL_MEMBER_HPP
#define MAKI_DETAIL_CALL_MEMBER_HPP

#include "type_traits.hpp"
#include "state_traits.hpp"
#include "submachine_fwd.hpp"
#include "overload_priority.hpp"
#include <type_traits>
#include <utility>

namespace maki::detail
{

#define MAKI_DETAIL_GENERATE_HAS_MEMBER_FUNCTION(name) /*NOLINT*/ \
    template<class State, class... Args> \
    constexpr auto has_##name##_impl(overload_priority::high /*unused*/) -> \
        decltype(std::declval<State>().name(std::declval<Args>()...), bool()) \
    { \
        return true; \
    } \
 \
    template<class State, class... Args> \
    constexpr bool has_##name##_impl(overload_priority::low /*unused*/) \
    { \
        return false; \
    } \
 \
    template<class State, class... Args> \
    constexpr bool has_##name() \
    { \
        return has_##name##_impl<State, Args...>(overload_priority::probe); \
    }

MAKI_DETAIL_GENERATE_HAS_MEMBER_FUNCTION(on_entry)
MAKI_DETAIL_GENERATE_HAS_MEMBER_FUNCTION(on_event)
MAKI_DETAIL_GENERATE_HAS_MEMBER_FUNCTION(on_event_ce)
MAKI_DETAIL_GENERATE_HAS_MEMBER_FUNCTION(on_event_mce)
MAKI_DETAIL_GENERATE_HAS_MEMBER_FUNCTION(on_exit)

#undef MAKI_DETAIL_GENERATE_HAS_MEMBER_FUNCTION

template<class Fn, class Machine, class Event>
void call_me(Fn&& fun, Machine& mach, const Event& event)
{
    if constexpr(std::is_invocable_v<Fn, Machine&, const Event&>)
    {
        fun(mach, event);
    }
    else if constexpr(std::is_invocable_v<Fn, const Event&>)
    {
        fun(event);
    }
    else if constexpr(is_nullary_v<Fn>)
    {
        fun();
    }
    else
    {
        constexpr auto is_false = sizeof(Machine) == 0;
        static_assert(is_false, "No valid signature found");
    }
}

template<class State, class Sm, class Event>
void call_on_entry
(
    [[maybe_unused]] State& state,
    [[maybe_unused]] Sm& mach,
    [[maybe_unused]] const Event& event
)
{
    if constexpr(state_traits::requires_on_entry<State>())
    {
        if constexpr(has_on_entry<State&, Sm&, const Event&>())
        {
            state.on_entry(mach, event);
        }
        else if constexpr(has_on_entry<State&, const Event&>())
        {
            state.on_entry(event);
        }
        else if constexpr(has_on_entry<State&>())
        {
            state.on_entry();
        }
        else
        {
            constexpr auto is_false = sizeof(Sm) == 0;
            static_assert(is_false, "No valid on_entry() signature found in state");
        }
    }
}

template<class State, class Sm, class Context, class Event>
void call_on_event(State& state, Sm& mach, Context& ctx, const Event& event)
{
    if constexpr(has_on_event<State&, const Event&>())
    {
        state.on_event(event);
    }
    else if constexpr(has_on_event_ce<State&, Context&, const Event&>())
    {
        state.on_event_ce(ctx, event);
    }
    else if constexpr(has_on_event_mce<State&, Sm&, Context&, const Event&>())
    {
        state.on_event_mce(mach, ctx, event);
    }
    else
    {
        constexpr auto is_false = sizeof(Sm) == 0;
        static_assert(is_false, "No valid on_event() signature found in state");
    }
}

template<class State, class Sm, class Context, class Event>
void call_on_event(State& state, Sm& /*mach*/, Context& /*ctx*/, const Event& event, bool& processed)
{
    if constexpr(state_traits::is_submachine_v<State>)
    {
        state.on_event(event, processed);
    }
    else
    {
        state.on_event(event);
        processed = true;
    }
}

template<class State, class Sm, class Event>
void call_on_exit
(
    [[maybe_unused]] State& state,
    [[maybe_unused]] Sm& mach,
    [[maybe_unused]] const Event& event
)
{
    if constexpr(state_traits::requires_on_exit<State>())
    {
        if constexpr(has_on_exit<State&, Sm&, const Event&>())
        {
            state.on_exit(mach, event);
        }
        else if constexpr(has_on_exit<State&, const Event&>())
        {
            state.on_exit(event);
        }
        else if constexpr(has_on_exit<State&>())
        {
            state.on_exit();
        }
        else
        {
            constexpr auto is_false = sizeof(Sm) == 0;
            static_assert(is_false, "No valid on_exit() signature found in state");
        }
    }
}

template<class ActionOrGuard, class Sm, class Context, class Event>
auto call_action_or_guard
(
    const ActionOrGuard& action_or_guard,
    [[maybe_unused]] Sm& mach,
    [[maybe_unused]] Context& ctx,
    [[maybe_unused]] const Event& event
)
{
    using fn_t = std::decay_t<ActionOrGuard>;

    if constexpr(std::is_invocable_v<fn_t, Sm&, Context&, const Event&>)
    {
        return action_or_guard(mach, ctx, event);
    }
    else if constexpr(std::is_invocable_v<fn_t, Context&, const Event&>)
    {
        return action_or_guard(ctx, event);
    }
    else if constexpr(std::is_invocable_v<fn_t, Context&>)
    {
        return action_or_guard(ctx);
    }
    else if constexpr(is_nullary_v<fn_t>)
    {
        return action_or_guard();
    }
    else
    {
        constexpr auto is_false = sizeof(Sm) == 0;
        static_assert(is_false, "No valid signature found for action/guard");
    }
}

} //namespace

#endif
