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

template<class State, class Event>
auto call_on_entry(State& state, const Event* pevent) ->
    decltype(state.on_entry(*pevent))
{
    state.on_entry(*pevent);
}

template<class State>
auto call_on_entry(State& state, const void* /*pevent*/) ->
    decltype(state.on_entry())
{
    state.on_entry();
}

template<class State, class Event>
void call_on_event(State& state, const Event& event)
{
    state.on_event(event);
}

template<class State, class Event>
auto call_on_exit(State& state, const Event* pevent) ->
    decltype(state.on_exit(*pevent))
{
    state.on_exit(*pevent);
}

template<class State>
auto call_on_exit(State& state, const void* /*pevent*/) ->
    decltype(state.on_exit())
{
    state.on_exit();
}

template<class Fn, class Sm, class Context, class Event>
auto call_action_or_guard(const Fn& fun, Sm* pmach, Context* pctx, const Event* pevent) ->
    decltype(fun(*pmach, *pctx, *pevent))
{
    return fun(*pmach, *pctx, *pevent);
}

template<class Fn, class Context, class Event>
auto call_action_or_guard(const Fn& fun, void* /*pmach*/, Context* pctx, const Event* pevent) ->
    decltype(fun(*pctx, *pevent))
{
    return fun(*pctx, *pevent);
}

template<class Fn, class Context>
auto call_action_or_guard(const Fn& fun, void* /*pmach*/, Context* pctx, const void* /*pevent*/) ->
    decltype(fun(*pctx))
{
    return fun(*pctx);
}

} //namespace

#endif
