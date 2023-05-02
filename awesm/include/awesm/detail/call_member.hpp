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
#include <type_traits>
#include <utility>

namespace awesm::detail
{

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
        call_on_entry_impl(state, &mach, &event);
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
auto call_on_exit_impl(State& state, Sm* pmach, const Event* pevent) ->
    decltype(state.on_exit(*pmach, *pevent))
{
    state.on_exit(*pmach, *pevent);
}

template<class State, class Event>
auto call_on_exit_impl(State& state, void* /*pmach*/, const Event* pevent) ->
    decltype(state.on_exit(*pevent))
{
    state.on_exit(*pevent);
}

template<class State>
auto call_on_exit_impl(State& state, void* /*pmach*/, const void* /*pevent*/) ->
    decltype(state.on_exit())
{
    state.on_exit();
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
        call_on_exit_impl(state, &mach, &event);
    }
}

template<class Fn, class Sm, class Context, class Event>
auto call_action_or_guard
(
    const Fn& fun,
    [[maybe_unused]] Sm& mach,
    [[maybe_unused]] Context& ctx,
    [[maybe_unused]] const Event& event
)
{
    if constexpr(std::is_invocable_v<Fn, Sm&, Context&, const Event&>)
    {
        return fun(mach, ctx, event);
    }
    else if constexpr(std::is_invocable_v<Fn, Context&, const Event&>)
    {
        return fun(ctx, event);
    }
    else if constexpr(std::is_invocable_v<Fn, Context&>)
    {
        return fun(ctx);
    }
    else if constexpr(is_nullary_v<Fn>)
    {
        return fun();
    }
}

} //namespace

#endif
