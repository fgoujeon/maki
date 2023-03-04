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
        on_entry_any,
        on_event,
        on_exit_any
    };
}

template<class... Options>
struct state_conf_tpl
{
    template<bool B>
    using set_on_entry_any_enabled = detail::tlu::set_at_f_t
    <
        state_conf_tpl,
        detail::state_option::on_entry_any,
        detail::constant<B>
    >;
    using on_entry_any = set_on_entry_any_enabled<true>;
    using no_on_entry_any = set_on_entry_any_enabled<false>;

    template<class... EventFilters>
    using on_event = detail::tlu::set_at_f_t
    <
        state_conf_tpl,
        detail::state_option::on_event,
        detail::tlu::type_list<EventFilters...>
    >;
    using on_event_any = on_event<any>;

    template<bool B>
    using set_on_exit_any_enabled = detail::tlu::set_at_f_t
    <
        state_conf_tpl,
        detail::state_option::on_exit_any,
        detail::constant<B>
    >;
    using on_exit_any = set_on_exit_any_enabled<true>;
    using no_on_exit_any = set_on_exit_any_enabled<false>;

    template<bool B>
    using set_get_pretty_name_enabled = detail::tlu::set_at_f_t
    <
        state_conf_tpl,
        detail::state_option::get_pretty_name,
        detail::constant<B>
    >;
    using get_pretty_name = set_get_pretty_name_enabled<true>;
    using no_get_pretty_name = set_get_pretty_name_enabled<false>;

    static constexpr auto is_composite = false;
};

using state_conf = state_conf_tpl<void, void, void, void>
    ::on_event<none>
    ::no_get_pretty_name
    ::no_on_entry_any
    ::no_on_exit_any
;

} //namespace

#endif
