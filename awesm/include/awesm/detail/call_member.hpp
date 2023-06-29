//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_without_event.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_CALL_MEMBER_HPP
#define AWESM_DETAIL_CALL_MEMBER_HPP

#include "type_traits.hpp"
#include "state_traits.hpp"
#include "subsm_fwd.hpp"
#include "overload_priority.hpp"
#include <type_traits>
#include <utility>

namespace awesm::detail
{

#define AWESM_DETAIL_GENERATE_HAS_MEMBER_FUNCTION(name) /*NOLINT*/ \
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

AWESM_DETAIL_GENERATE_HAS_MEMBER_FUNCTION(on_entry)
AWESM_DETAIL_GENERATE_HAS_MEMBER_FUNCTION(on_exit)

#undef AWESM_DETAIL_GENERATE_HAS_MEMBER_FUNCTION

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

template<const auto& Fn, class Sm, class Region, class Context, class Event>
auto call_action_or_guard
(
    [[maybe_unused]] Sm& mach,
    [[maybe_unused]] Region& reg,
    [[maybe_unused]] Context& ctx,
    [[maybe_unused]] const Event& event
)
{
    using fn_t = std::decay_t<decltype(Fn)>;

    if constexpr(std::is_invocable_v<fn_t, Sm&, Region&, Context&, const Event&>)
    {
        return Fn(mach, reg, ctx, event);
    }
    else if constexpr(std::is_invocable_v<fn_t, Region&, const Event&>)
    {
        return Fn(reg, event);
    }
    else if constexpr(std::is_invocable_v<fn_t, Sm&, Context&, const Event&>)
    {
        return Fn(mach, ctx, event);
    }
    else if constexpr(std::is_invocable_v<fn_t, Context&, const Event&>)
    {
        return Fn(ctx, event);
    }
    else if constexpr(std::is_invocable_v<fn_t, Context&>)
    {
        return Fn(ctx);
    }
    else if constexpr(is_nullary_v<fn_t>)
    {
        return Fn();
    }
    else
    {
        constexpr auto is_false = sizeof(Sm) == 0;
        static_assert(is_false, "No valid signature found for action/guard");
    }
}

} //namespace

#endif
