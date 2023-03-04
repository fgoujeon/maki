//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SM_CONF_HPP
#define AWESM_SM_CONF_HPP

#include "transition_table.hpp"
#include "type_filters.hpp"
#include "detail/constant_list.hpp"
#include "detail/constant.hpp"
#include "detail/tlu.hpp"

namespace awesm
{

namespace detail
{
    enum class sm_option
    {
        //Common with state_option, don't reorder
        get_pretty_name,
        on_entry,
        on_event,
        on_exit,

        //Specific
        after_state_transition,
        auto_start,
        before_entry,
        before_state_transition,
        context,
        on_exception,
        run_to_completion,
        small_event_max_align,
        small_event_max_size,
        transition_tables,
    };
}

template<class... Options>
struct sm_conf_tpl
{
    template<bool B>
    using after_state_transition = detail::tlu::set_at_f_t
    <
        sm_conf_tpl,
        detail::sm_option::after_state_transition,
        detail::constant<B>
    >;

    template<bool B>
    using auto_start = detail::tlu::set_at_f_t
    <
        sm_conf_tpl,
        detail::sm_option::auto_start,
        detail::constant<B>
    >;

    template<bool B>
    using before_entry = detail::tlu::set_at_f_t
    <
        sm_conf_tpl,
        detail::sm_option::before_entry,
        detail::constant<B>
    >;

    template<bool B>
    using before_state_transition = detail::tlu::set_at_f_t
    <
        sm_conf_tpl,
        detail::sm_option::before_state_transition,
        detail::constant<B>
    >;

    template<class T>
    using context = detail::tlu::set_at_f_t<sm_conf_tpl, detail::sm_option::context, T>;

    template<bool B>
    using run_to_completion = detail::tlu::set_at_f_t
    <
        sm_conf_tpl,
        detail::sm_option::run_to_completion,
        detail::constant<B>
    >;

    template<bool B>
    using get_pretty_name = detail::tlu::set_at_f_t
    <
        sm_conf_tpl,
        detail::sm_option::get_pretty_name,
        detail::constant<B>
    >;

    template<bool B>
    using on_exception = detail::tlu::set_at_f_t
    <
        sm_conf_tpl,
        detail::sm_option::on_exception,
        detail::constant<B>
    >;

    template<std::size_t Value>
    using small_event_max_align = detail::tlu::set_at_f_t
    <
        sm_conf_tpl,
        detail::sm_option::small_event_max_align,
        detail::constant<Value>
    >;

    template<std::size_t Value>
    using small_event_max_size = detail::tlu::set_at_f_t
    <
        sm_conf_tpl,
        detail::sm_option::small_event_max_size,
        detail::constant<Value>
    >;

    template<class... EventFilters>
    using on_event = detail::tlu::set_at_f_t
    <
        sm_conf_tpl,
        detail::sm_option::on_event,
        detail::tlu::type_list<EventFilters...>
    >;

    template<bool B>
    using on_entry = detail::tlu::set_at_f_t
    <
        sm_conf_tpl,
        detail::sm_option::on_entry,
        detail::constant<B>
    >;

    template<bool B>
    using on_exit = detail::tlu::set_at_f_t
    <
        sm_conf_tpl,
        detail::sm_option::on_exit,
        detail::constant<B>
    >;

    template<auto... Fns>
    using transition_tables = detail::tlu::set_at_f_t
    <
        sm_conf_tpl,
        detail::sm_option::transition_tables,
        detail::constant_list<Fns...>
    >;

    static constexpr auto is_composite = true;
};

inline constexpr auto small_event_max_align = 8;
inline constexpr auto small_event_max_size = 16;

using sm_conf = sm_conf_tpl<void, void, void, void, void, void, void, void, void, void, void, void, void, void>
    ::after_state_transition<false>
    ::auto_start<true>
    ::before_entry<false>
    ::before_state_transition<false>
    ::get_pretty_name<false>
    ::on_entry<false>
    ::on_event<none>
    ::on_exception<false>
    ::on_exit<false>
    ::run_to_completion<true>
    ::small_event_max_align<small_event_max_align>
    ::small_event_max_size<small_event_max_size>
;

} //namespace

#endif
