//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_STATE_TRAITS_HPP
#define AWESM_DETAIL_STATE_TRAITS_HPP

#include "../state_conf.hpp"

namespace awesm::detail::state_traits
{

template<template<class> class Option, class Event, class TypePattern>
constexpr bool requires_on_xxx_conf(Option<TypePattern>* /*tag*/)
{
    return TypePattern::template matches<Event>;
}

template<template<class> class Option, class Event>
constexpr bool requires_on_xxx_conf(void* /*tag*/)
{
    return false;
}

template<class State, class Event>
constexpr auto requires_on_entry_v = requires_on_xxx_conf
<
    state_options::on_entry,
    Event
>(static_cast<typename State::conf*>(nullptr));

template<class State, class Event>
constexpr auto requires_on_event_v = requires_on_xxx_conf
<
    state_options::on_event,
    Event
>(static_cast<typename State::conf*>(nullptr));

template<class State, class Event>
constexpr auto requires_on_exit_v = requires_on_xxx_conf
<
    state_options::on_exit,
    Event
>(static_cast<typename State::conf*>(nullptr));

} //namespace

#endif
