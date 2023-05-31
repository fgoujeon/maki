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
auto call_on_entry_impl(State& state, Sm* pmach, const Event* pevent) ->
    decltype(state.on_entry(*pmach, *pevent))
{
    state.on_entry(*pmach, *pevent);
}

template<class State, class Event>
auto call_on_entry_impl(State& state, void* /*pmach*/, const Event* pevent) ->
    decltype(state.on_entry(*pevent))
{
    state.on_entry(*pevent);
}

template<class State>
auto call_on_entry_impl(State& state, void* /*pmach*/, const void* /*pevent*/) ->
    decltype(state.on_entry())
{
    state.on_entry();
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
            constexpr auto is_false = sizeof(State) == 0;
            static_assert(is_false, "No valid on_entry() signature found in state");
        }
    }
}

template<class State>
struct call_on_event_helper
{
    template<class Event>
    static bool call(State& state, const Event& event)
    {
        state.on_event(event);
        return true;
    }
};

template<class Def, class ParentRegion>
struct call_on_event_helper<subsm<Def, ParentRegion>>
{
    template<class Event>
    static bool call(subsm<Def, ParentRegion>& state, const Event& event)
    {
        return state.on_event(event);
    }
};

template<class State, class Event>
bool call_on_event(State& state, const Event& event)
{
    return call_on_event_helper<State>::call(state, event);
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
            constexpr auto is_false = sizeof(State) == 0;
            static_assert(is_false, "No valid on_exit() signature found in state");
        }
    }
}

template<const auto& Fn, class Sm, class Context, class Event>
auto call_action_or_guard
(
    [[maybe_unused]] Sm& mach,
    [[maybe_unused]] Context& ctx,
    [[maybe_unused]] const Event& event
)
{
    using fn_t = std::decay_t<decltype(Fn)>;

    if constexpr(std::is_invocable_v<fn_t, Sm&, Context&, const Event&>)
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
        constexpr auto is_false = sizeof(Context) == 0;
        static_assert(is_false, "No valid signature found for action/guard");
    }
}

} //namespace

#endif
