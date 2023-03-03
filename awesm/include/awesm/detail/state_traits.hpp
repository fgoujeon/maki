//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_STATE_TRAITS_HPP
#define AWESM_DETAIL_STATE_TRAITS_HPP

#include "subsm_fwd.hpp"
#include "tlu.hpp"
#include "../type_filters.hpp"
#include "../state_conf.hpp"
#include "../sm_conf.hpp"

namespace awesm::detail::state_traits
{

//wrap

template<class State, class Region, class Enable = void>
struct wrap
{
    using type = State;
};

template<class State, class Region>
struct wrap<State, Region, std::enable_if_t<State::conf::is_composite>>
{
    using type = subsm<State, Region>;
};

template<class State, class Region>
using wrap_t = typename wrap<State, Region>::type;


//other

template<class Conf>
struct requires_on_entry;

template<class... Options>
struct requires_on_entry<awesm::sm_conf_tpl<Options...>>
{
    static constexpr auto value = tlu::at_f_t<sm_conf_tpl<Options...>, sm_option::on_entry_any>::value;
};

template<class... Options>
struct requires_on_entry<awesm::state_conf_tpl<Options...>>
{
    static constexpr auto value = tlu::at_f_t<state_conf_tpl<Options...>, state_option::on_entry_any>::value;
};

template<class State>
constexpr auto requires_on_entry_v = requires_on_entry<typename State::conf>::value;

template<class Conf, class Event>
struct requires_on_event;

template<class... Options, class Event>
struct requires_on_event<awesm::sm_conf_tpl<Options...>, Event>
{
    using filter_type_list = tlu::at_f_t<sm_conf_tpl<Options...>, sm_option::on_event>;
    static constexpr auto value = matches_any_filter_v<Event, filter_type_list>;
};

template<class... Options, class Event>
struct requires_on_event<awesm::state_conf_tpl<Options...>, Event>
{
    using filter_type_list = tlu::at_f_t<state_conf_tpl<Options...>, state_option::on_event>;
    static constexpr auto value = matches_any_filter_v<Event, filter_type_list>;
};

template<class State, class Event>
constexpr auto requires_on_event_v = requires_on_event<typename State::conf, Event>::value;

template<class Conf>
struct requires_on_exit;

template<class... Options>
struct requires_on_exit<awesm::sm_conf_tpl<Options...>>
{
    static constexpr auto value = tlu::at_f_t<sm_conf_tpl<Options...>, sm_option::on_exit_any>::value;
};

template<class... Options>
struct requires_on_exit<awesm::state_conf_tpl<Options...>>
{
    static constexpr auto value = tlu::at_f_t<state_conf_tpl<Options...>, state_option::on_exit_any>::value;
};

template<class State>
constexpr auto requires_on_exit_v = requires_on_exit<typename State::conf>::value;

} //namespace

#endif
