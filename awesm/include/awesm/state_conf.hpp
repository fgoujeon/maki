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
private:
    template<detail::state_option Option, class... Ts>
    using set_types = detail::tlu::set_f_t
    <
        state_conf_tpl,
        Option,
        detail::tlu::type_list<Ts...>
    >;

    template<detail::state_option Option, auto C>
    using set_constant = detail::tlu::set_f_t
    <
        state_conf_tpl,
        Option,
        detail::constant<C>
    >;

public:
    template<bool Enable = true>
    using on_entry = set_constant<detail::state_option::on_entry, Enable>;

    template<class... EventFilters>
    using on_event = set_types<detail::state_option::on_event, EventFilters...>;

    template<bool Enable = true>
    using on_event_auto = set_constant<detail::state_option::on_event_auto, Enable>;

    template<bool Enable = true>
    using on_exit = set_constant<detail::state_option::on_exit, Enable>;

    template<bool Enable = true>
    using get_pretty_name = set_constant<detail::state_option::get_pretty_name, Enable>;

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
