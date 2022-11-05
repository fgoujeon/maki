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

template<class... Options>
struct event_option_pattern;

template<class Option, class... Options>
struct event_option_pattern<Option, Options...>
{
    using type = typename event_option_pattern<Options...>::type;
};

template<class TypePattern, class... Options>
struct event_option_pattern<state_options::on_event<TypePattern>, Options...>
{
    using type = TypePattern;
};

template<>
struct event_option_pattern<>
{
    using type = any_of<>;
};

template<class Conf, class Event>
struct requires_on_event_conf;

template<class... Options, class Event>
struct requires_on_event_conf<state_conf<Options...>, Event>
{
    using pattern_t = typename event_option_pattern<Options...>::type;
    static constexpr auto value = pattern_t::template matches<Event>;
};

template<class State, class Event>
struct requires_on_event
{
    static constexpr auto value = requires_on_event_conf
    <
        typename State::conf,
        Event
    >::value;
};

template<class State, class Event>
constexpr auto requires_on_event_v = requires_on_event<State, Event>::value;

} //namespace

#endif
