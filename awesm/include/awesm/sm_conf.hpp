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

namespace sm_opts
{
    template<bool Enable = true>
    using after_state_transition = detail::conf_element
    <
        detail::option_id::after_state_transition,
        detail::constant<Enable>
    >;

    template<bool Enable = true>
    using auto_start = detail::conf_element<detail::option_id::auto_start, detail::constant<Enable>>;

    template<bool Enable = true>
    using before_entry = detail::conf_element<detail::option_id::before_entry, detail::constant<Enable>>;

    template<bool Enable = true>
    using before_state_transition = detail::conf_element
    <
        detail::option_id::before_state_transition,
        detail::constant<Enable>
    >;

    template<class T>
    using context = detail::conf_element<detail::option_id::context, T>;

    template<bool Enable = true>
    using run_to_completion = detail::conf_element
    <
        detail::option_id::run_to_completion,
        detail::constant<Enable>
    >;

    template<bool Enable = true>
    using get_pretty_name = detail::conf_element
    <
        detail::option_id::get_pretty_name,
        detail::constant<Enable>
    >;

    template<bool Enable = true>
    using on_exception = detail::conf_element<detail::option_id::on_exception, detail::constant<Enable>>;

    template<bool Enable = true>
    using on_unprocessed = detail::conf_element<detail::option_id::on_unprocessed, detail::constant<Enable>>;

    template<std::size_t Value>
    using small_event_max_align = detail::conf_element
    <
        detail::option_id::small_event_max_align,
        detail::constant<Value>
    >;

    template<std::size_t Value>
    using small_event_max_size = detail::conf_element
    <
        detail::option_id::small_event_max_size,
        detail::constant<Value>
    >;

    template<class... EventFilters>
    using on_event = detail::conf_element<detail::option_id::on_event, detail::tlu::type_list<EventFilters...>>;

    using on_event_auto = detail::conf_element<detail::option_id::on_event_auto, detail::constant<true>>;

    template<bool Enable = true>
    using on_entry = detail::conf_element<detail::option_id::on_entry, detail::constant<Enable>>;

    template<bool Enable = true>
    using on_exit = detail::conf_element<detail::option_id::on_exit, detail::constant<Enable>>;

    template<class... Ts>
    using transition_tables = detail::conf_element<detail::option_id::transition_tables, detail::tlu::type_list<Ts...>>;
}

template<class... Options>
class sm_conf
{
public:
    static constexpr auto is_composite = true;
};

} //namespace

#endif
