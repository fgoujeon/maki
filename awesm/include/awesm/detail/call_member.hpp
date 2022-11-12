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

namespace awesm
{

template<class Definition>
class composite_state;

} //namespace

namespace awesm::detail
{

template<class T>
struct is_composite_state
{
    static constexpr auto value = false;
};

template<class Definition>
struct is_composite_state<composite_state<Definition>>
{
    static constexpr auto value = true;
};

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

template<class RegionPath, class State, class Sm, class Event>
void call_on_entry(State& state, Sm& mach, const Event& event)
{
    if constexpr(is_composite_state<State>::value)
    {
        state.template on_entry<RegionPath>(mach, event);
    }
    else if constexpr
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

template<class RegionPath, class State, class Sm, class Event>
void call_on_event(State& state, Sm& mach, const Event& event)
{
    if constexpr(is_composite_state<State>::value)
    {
        state.template on_event<RegionPath>(mach, event);
    }
    else
    {
        state.on_event(event);
    }
}

template<class RegionPath, class State, class Sm, class Event>
void call_on_exit(State& state, Sm& mach, const Event& event)
{
    if constexpr(is_composite_state<State>::value)
    {
        state.template on_exit<RegionPath>(mach, event);
    }
    else if constexpr
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
