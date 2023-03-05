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
        on_event_auto,
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
class sm_conf_tpl
{
private:
    template<detail::sm_option Option, class T>
    using set_type = detail::tlu::set_at_f_t
    <
        sm_conf_tpl,
        Option,
        T
    >;

    template<detail::sm_option Option, class... Ts>
    using set_types = detail::tlu::set_at_f_t
    <
        sm_conf_tpl,
        Option,
        detail::tlu::type_list<Ts...>
    >;

    template<detail::sm_option Option, auto C>
    using set_constant = detail::tlu::set_at_f_t
    <
        sm_conf_tpl,
        Option,
        detail::constant<C>
    >;

public:
    template<bool Enable = true>
    using after_state_transition = set_constant
    <
        detail::sm_option::after_state_transition,
        Enable
    >;

    template<bool Enable = true>
    using auto_start = set_constant<detail::sm_option::auto_start, Enable>;

    template<bool Enable = true>
    using before_entry = set_constant<detail::sm_option::before_entry, Enable>;

    template<bool Enable = true>
    using before_state_transition = set_constant
    <
        detail::sm_option::before_state_transition,
        Enable
    >;

    template<class T>
    using context = set_type<detail::sm_option::context, T>;

    template<bool Enable = true>
    using run_to_completion = set_constant
    <
        detail::sm_option::run_to_completion,
        Enable
    >;

    template<bool Enable = true>
    using get_pretty_name = set_constant
    <
        detail::sm_option::get_pretty_name,
        Enable
    >;

    template<bool Enable = true>
    using on_exception = set_constant<detail::sm_option::on_exception, Enable>;

    template<std::size_t Value>
    using small_event_max_align = set_constant
    <
        detail::sm_option::small_event_max_align,
        Value
    >;

    template<std::size_t Value>
    using small_event_max_size = set_constant
    <
        detail::sm_option::small_event_max_size,
        Value
    >;

    template<class... EventFilters>
    using on_event = set_types<detail::sm_option::on_event, EventFilters...>;

    template<bool Enable = true>
    using on_event_auto = set_constant<detail::sm_option::on_event_auto, Enable>;

    template<bool Enable = true>
    using on_entry = set_constant<detail::sm_option::on_entry, Enable>;

    template<bool Enable = true>
    using on_exit = set_constant<detail::sm_option::on_exit, Enable>;

    template<class... Ts>
    using transition_tables = set_types<detail::sm_option::transition_tables, Ts...>;

    static constexpr auto is_composite = true;
};

inline constexpr auto small_event_max_align = 8;
inline constexpr auto small_event_max_size = 16;

using sm_conf = sm_conf_tpl<void, void, void, void, void, void, void, void, void, void, void, void, void, void, void>
    ::after_state_transition<false>
    ::auto_start<true>
    ::before_entry<false>
    ::before_state_transition<false>
    ::get_pretty_name<false>
    ::on_entry<false>
    ::on_event<none>
    ::on_event_auto<false>
    ::on_exception<false>
    ::on_exit<false>
    ::run_to_completion<true>
    ::small_event_max_align<small_event_max_align>
    ::small_event_max_size<small_event_max_size>
;

} //namespace

#endif
