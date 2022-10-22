//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_without_event.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_CALL_MEMBER_HPP
#define AWESM_DETAIL_CALL_MEMBER_HPP

#include "ignore_unused.hpp"
#include "type_traits.hpp"
#include <type_traits>
#include <utility>

namespace awesm::detail
{

/*
A 0 must be passed to int and long dummy parameters.
We use this trick so that int overloads take precedence over long ones.
*/

template<class State, class SmConfiguration, class Event>
auto call_on_entry(State& state, SmConfiguration& sm_conf, const Event& event)
{
    if constexpr(is_composite_state_v<State>)
    {
        state.on_entry(sm_conf, event);
    }
    else if constexpr(std::is_empty_v<State>)
    {
        //nothing
    }
    else
    {
        state.on_entry(event);
    }
}

template<class State, class SmConfiguration, class Event>
auto call_on_exit(State& state, SmConfiguration& sm_conf, const Event& event)
{
    if constexpr(is_composite_state_v<State>)
    {
        state.on_exit(sm_conf, event);
    }
    else if constexpr(std::is_empty_v<State>)
    {
        //nothing
    }
    else
    {
        state.on_exit(event);
    }
}

} //namespace

#endif
