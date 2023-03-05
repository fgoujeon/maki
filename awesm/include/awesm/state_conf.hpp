//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_STATE_CONF_HPP
#define AWESM_STATE_CONF_HPP

#include "type_filters.hpp"
#include "detail/constant.hpp"
#include "detail/tlu.hpp"

namespace awesm
{

namespace detail
{
    enum class state_option
    {
        //Common with state_option, don't reorder
        get_pretty_name,
        on_entry,
        on_event,
        on_event_auto,
        on_exit
    };
}

template<class... Options>
struct state_conf_tpl
{
    template<bool Enable = true>
    using on_entry = detail::tlu::set_at_f_t
    <
        state_conf_tpl,
        detail::state_option::on_entry,
        detail::constant<Enable>
    >;

    template<class... EventFilters>
    using on_event = detail::tlu::set_at_f_t
    <
        state_conf_tpl,
        detail::state_option::on_event,
        detail::tlu::type_list<EventFilters...>
    >;

    template<bool Enable = true>
    using on_event_auto = detail::tlu::set_at_f_t
    <
        state_conf_tpl,
        detail::state_option::on_event_auto,
        detail::constant<Enable>
    >;

    template<bool Enable = true>
    using on_exit = detail::tlu::set_at_f_t
    <
        state_conf_tpl,
        detail::state_option::on_exit,
        detail::constant<Enable>
    >;

    template<bool Enable = true>
    using get_pretty_name = detail::tlu::set_at_f_t
    <
        state_conf_tpl,
        detail::state_option::get_pretty_name,
        detail::constant<Enable>
    >;

    static constexpr auto is_composite = false;
};

using state_conf = state_conf_tpl<void, void, void, void, void>
    ::get_pretty_name<false>
    ::on_entry<false>
    ::on_event<none>
    ::on_event_auto<false>
    ::on_exit<false>
;

} //namespace

#endif
