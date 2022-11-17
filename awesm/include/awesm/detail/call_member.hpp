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

template<const auto& Fn, class Sm, class Context, class Event>
auto call_action_or_guard(Sm* pmach, Context* pctx, const Event* pevent) ->
    decltype(Fn(*pmach, *pctx, *pevent))
{
    return Fn(*pmach, *pctx, *pevent);
}

template<const auto& Fn, class Context, class Event>
auto call_action_or_guard(void* /*pmach*/, Context* pctx, const Event* pevent) ->
    decltype(Fn(*pctx, *pevent))
{
    return Fn(*pctx, *pevent);
}

template<const auto& Fn, class Context>
auto call_action_or_guard(void* /*pmach*/, Context* pctx, const void* /*pevent*/) ->
    decltype(Fn(*pctx))
{
    return Fn(*pctx);
}

} //namespace

#endif
