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

template<class T, class F>
constexpr auto is_valid(const F& /*f*/, int /*high_overload_priority*/) -> decltype(std::declval<F>()(std::declval<T>()), bool())
{
    return true;
}

template<class T, class F>
constexpr bool is_valid(const F& /*f*/, long /*low_overload_priority*/)
{
    return false;
}

template<class State, class Event>
void call_on_entry(State& state, const Event& event)
{
    //VS2017 is stupid
    detail::ignore_unused(event);

    if constexpr
    (
        is_valid<State&>
        (
            [](auto& state) -> decltype(state.on_entry(std::declval<const Event&>()))
            {
            },
            0
        )
    )
    {
        state.on_entry(event);
    }
    else
    {
        state.on_entry();
    }
}

template<class State, class Event>
void call_on_event(State& state, const Event& event)
{
    state.on_event(event);
}

template<class State, class Event>
void call_on_exit(State& state, const Event& event)
{
    //VS2017 is stupid
    detail::ignore_unused(event);

    if constexpr
    (
        is_valid<State&>
        (
            [](auto& state) -> decltype(state.on_exit(std::declval<const Event&>()))
            {
            },
            0
        )
    )
    {
        state.on_exit(event);
    }
    else
    {
        state.on_exit();
    }
}

template<class Action, class Event>
auto call_execute(Action* paction, const Event* pevent) ->
    decltype(paction->execute(*pevent))
{
    paction->execute(*pevent);
}

template<class Action>
auto call_execute(Action* paction, const void* /*pevent*/) ->
    decltype(paction->execute())
{
    paction->execute();
}

template<class Guard, class Event>
auto call_check(Guard* pguard, const Event* pevent) ->
    decltype(pguard->check(*pevent))
{
    return pguard->check(*pevent);
}

template<class Guard>
auto call_check(Guard* pguard, const void* /*pevent*/) ->
    decltype(pguard->check())
{
    return pguard->check();
}

} //namespace

#endif
