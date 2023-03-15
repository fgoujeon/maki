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
    using after_state_transition = detail::conf_element
    <
        detail::option_id::after_state_transition,
        detail::constant<true>
    >;

    using no_auto_start = detail::conf_element<detail::option_id::auto_start, detail::constant<false>>;

    using before_entry = detail::conf_element<detail::option_id::before_entry, detail::constant<true>>;

    using before_state_transition = detail::conf_element
    <
        detail::option_id::before_state_transition,
        detail::constant<true>
    >;

    template<class T>
    using context = detail::conf_element<detail::option_id::context, T>;

    using no_run_to_completion = detail::conf_element
    <
        detail::option_id::run_to_completion,
        detail::constant<false>
    >;

    using get_pretty_name = detail::conf_element
    <
        detail::option_id::get_pretty_name,
        detail::constant<true>
    >;

    using on_exception = detail::conf_element<detail::option_id::on_exception, detail::constant<true>>;

    using on_unprocessed = detail::conf_element<detail::option_id::on_unprocessed, detail::constant<true>>;

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
struct sm_conf
{
    using after_state_transition = sm_conf<Options..., sm_opts::after_state_transition>;

    using no_auto_start = sm_conf<Options..., sm_opts::no_auto_start>;

    using before_entry = sm_conf<Options..., sm_opts::before_entry>;

    using before_state_transition = sm_conf<Options..., sm_opts::before_state_transition>;

    template<class T>
    using context = sm_conf<Options..., sm_opts::context<T>>;

    using no_run_to_completion = sm_conf<Options..., sm_opts::no_run_to_completion>;

    using get_pretty_name = sm_conf<Options..., sm_opts::get_pretty_name>;

    using on_exception = sm_conf<Options..., sm_opts::on_exception>;

    using on_unprocessed = sm_conf<Options..., sm_opts::on_unprocessed>;

    template<std::size_t Value>
    using small_event_max_align = sm_conf<Options..., sm_opts::small_event_max_align<Value>>;

    template<std::size_t Value>
    using small_event_max_size = sm_conf<Options..., sm_opts::small_event_max_size<Value>>;

    template<class... EventFilters>
    using on_event = sm_conf<Options..., sm_opts::on_event<EventFilters...>>;

    using on_event_auto = sm_conf<Options..., sm_opts::on_event_auto>;

    template<bool Enable = true>
    using on_entry = sm_conf<Options..., sm_opts::on_entry<Enable>>;

    template<bool Enable = true>
    using on_exit = sm_conf<Options..., sm_opts::on_exit<Enable>>;

    template<class... Ts>
    using transition_tables = sm_conf<Options..., sm_opts::transition_tables<Ts...>>;

    static constexpr auto is_composite = true;
};

} //namespace

#endif
