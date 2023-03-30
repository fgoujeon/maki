//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SM_CONF_HPP
#define AWESM_SM_CONF_HPP

#include "transition_table.hpp"
#include "type_filters.hpp"
#include "detail/constant.hpp"
#include "detail/tlu.hpp"
#include "detail/conf.hpp"

namespace awesm
{

namespace subsm_opts
{
    template<class T>
    using context = detail::conf_element<detail::option_id::context, T>;

    using get_pretty_name = detail::conf_element
    <
        detail::option_id::get_pretty_name,
        detail::constant<true>
    >;

    template<class... EventFilters>
    using on_event = detail::conf_element<detail::option_id::on_event, detail::type_list<EventFilters...>>;

    using on_event_auto = detail::conf_element<detail::option_id::on_event_auto, detail::constant<true>>;

    template<bool Enable = true>
    using on_entry = detail::conf_element<detail::option_id::on_entry, detail::constant<Enable>>;

    template<bool Enable = true>
    using on_exit = detail::conf_element<detail::option_id::on_exit, detail::constant<Enable>>;

    template<class... Ts>
    using transition_tables = detail::conf_element<detail::option_id::transition_tables, detail::type_list<Ts...>>;
}

template<class... Options>
struct subsm_conf_tpl
{
    template<class T>
    using context = subsm_conf_tpl<Options..., subsm_opts::context<T>>;

    using get_pretty_name = subsm_conf_tpl<Options..., subsm_opts::get_pretty_name>;

    template<class... EventFilters>
    using on_event = subsm_conf_tpl<Options..., subsm_opts::on_event<EventFilters...>>;

    using on_event_auto = subsm_conf_tpl<Options..., subsm_opts::on_event_auto>;

    template<bool Enable = true>
    using on_entry = subsm_conf_tpl<Options..., subsm_opts::on_entry<Enable>>;

    template<bool Enable = true>
    using on_exit = subsm_conf_tpl<Options..., subsm_opts::on_exit<Enable>>;

    template<class... Ts>
    using transition_tables = subsm_conf_tpl<Options..., subsm_opts::transition_tables<Ts...>>;

    static constexpr auto is_composite = true;
};

using subsm_conf = subsm_conf_tpl<>;

} //namespace

#endif