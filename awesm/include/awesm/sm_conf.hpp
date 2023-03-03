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
#include "detail/type_list.hpp"
#include "detail/tlu.hpp"

namespace awesm
{

namespace detail
{
    template<auto Value>
    struct constant
    {
        static constexpr auto value = Value;
    };

    enum class sm_option
    {
        after_state_transition,
        auto_start,
        before_entry,
        before_state_transition,
        context,
        pretty_name,
        on_event,
        on_exception,
        run_to_completion,
        small_event_max_align,
        small_event_max_size,
        transition_table_list,
        on_entry_any,
        on_exit_any,
        _size //It's 2023 and we still have to do this :'(
    };
}

template<class... Options>
struct sm_conf_tpl
{
    template<bool B>
    using set_after_state_transition_enabled = detail::tlu::set_at_f_t
    <
        sm_conf_tpl,
        detail::sm_option::after_state_transition,
        detail::constant<B>
    >;
    using after_state_transition = set_after_state_transition_enabled<true>;
    using no_after_state_transition = set_after_state_transition_enabled<false>;

    template<bool B>
    using set_auto_start_enabled = detail::tlu::set_at_f_t
    <
        sm_conf_tpl,
        detail::sm_option::auto_start,
        detail::constant<B>
    >;
    using auto_start = set_auto_start_enabled<true>;
    using no_auto_start = set_auto_start_enabled<false>;

    template<bool B>
    using set_before_entry_enabled = detail::tlu::set_at_f_t
    <
        sm_conf_tpl,
        detail::sm_option::before_entry,
        detail::constant<B>
    >;
    using before_entry = set_before_entry_enabled<true>;
    using no_before_entry = set_before_entry_enabled<false>;

    template<bool B>
    using set_before_state_transition_enabled = detail::tlu::set_at_f_t
    <
        sm_conf_tpl,
        detail::sm_option::before_state_transition,
        detail::constant<B>
    >;
    using before_state_transition = set_before_state_transition_enabled<true>;
    using no_before_state_transition = set_before_state_transition_enabled<false>;

    template<class T>
    using context = detail::tlu::set_at_f_t<sm_conf_tpl, detail::sm_option::context, T>;

    template<bool B>
    using set_run_to_completion_enabled = detail::tlu::set_at_f_t
    <
        sm_conf_tpl,
        detail::sm_option::run_to_completion,
        detail::constant<B>
    >;
    using run_to_completion = set_run_to_completion_enabled<true>;
    using no_run_to_completion = set_run_to_completion_enabled<false>;

    template<bool B>
    using set_pretty_name_enabled = detail::tlu::set_at_f_t
    <
        sm_conf_tpl,
        detail::sm_option::pretty_name,
        detail::constant<B>
    >;
    using pretty_name = set_pretty_name_enabled<true>;
    using no_pretty_name = set_pretty_name_enabled<false>;

    template<bool B>
    using set_on_exception_enabled = detail::tlu::set_at_f_t
    <
        sm_conf_tpl,
        detail::sm_option::on_exception,
        detail::constant<B>
    >;
    using on_exception = set_on_exception_enabled<true>;
    using no_on_exception = set_on_exception_enabled<false>;

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
        detail::type_list<EventFilters...>
    >;

    template<bool B>
    using set_on_entry_enabled = detail::tlu::set_at_f_t
    <
        sm_conf_tpl,
        detail::sm_option::on_entry_any,
        detail::constant<B>
    >;
    using on_entry_any = set_on_entry_enabled<true>;
    using no_on_entry_any = set_on_entry_enabled<false>;

    template<bool B>
    using set_on_exit_enabled = detail::tlu::set_at_f_t
    <
        sm_conf_tpl,
        detail::sm_option::on_exit_any,
        detail::constant<B>
    >;
    using on_exit_any = set_on_exit_enabled<true>;
    using no_on_exit_any = set_on_exit_enabled<false>;

    template<auto... Fns>
    using transition_table_list = detail::tlu::set_at_f_t
    <
        sm_conf_tpl,
        detail::sm_option::transition_table_list,
        detail::constant_list<Fns...>
    >;

    template<auto Fn>
    using transition_table = transition_table_list<Fn>;

    static constexpr auto is_composite = true;
};

inline constexpr auto small_event_max_align = 8;
inline constexpr auto small_event_max_size = 16;

using sm_conf = sm_conf_tpl<void, void, void, void, void, void, void, void, void, void, void, void, void, void>
    ::auto_start
    ::no_after_state_transition
    ::no_before_entry
    ::no_before_state_transition
    ::no_on_entry_any
    ::no_on_exception
    ::no_on_exit_any
    ::no_pretty_name
    ::on_event<none>
    ::run_to_completion
    ::small_event_max_align<small_event_max_align>
    ::small_event_max_size<small_event_max_size>
;

} //namespace

#endif
