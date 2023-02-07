//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_STATE_TRAITS_HPP
#define AWESM_DETAIL_STATE_TRAITS_HPP

#include "subsm_wrapper_fwd.hpp"
#include "tlu.hpp"
#include "../state_conf.hpp"

namespace awesm::detail::state_traits
{

//wrap

template<class State, class RegionPath, class Enable = void>
struct wrap
{
    using type = State;
};

template<class State, class RegionPath>
struct wrap<State, RegionPath, std::enable_if_t<State::conf::is_composite>>
{
    using type = subsm_wrapper<RegionPath, State>;
};

template<class State, class RegionPath>
using wrap_t = typename wrap<State, RegionPath>::type;


//other

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

template<class State>
constexpr auto requires_on_entry_v =
    tlu::contains_v<typename State::conf::option_mix_type, state_opts::on_entry_any>
;

template<class State, class Event>
constexpr auto requires_on_event_v = requires_on_xxx_conf
<
    state_opts::on_event,
    Event
>(static_cast<typename State::conf::option_mix_type*>(nullptr));

template<class State>
constexpr auto requires_on_exit_v =
    tlu::contains_v<typename State::conf::option_mix_type, state_opts::on_exit_any>
;

} //namespace

#endif
