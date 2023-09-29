//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_STATE_CONF_HPP
#define MAKI_STATE_CONF_HPP

#include "detail/tlu.hpp"
#include "detail/type_list.hpp"
#include "detail/type.hpp"

namespace maki
{

namespace detail
{
    template<class... Args>
    constexpr auto make_state_conf(const Args&... args);
}

template<class OnEventTypeList = detail::type_list<>>
struct state_conf
{
    bool has_on_entry_any = false; //NOLINT(misc-non-private-member-variables-in-classes)
    bool on_event_auto = false; //NOLINT(misc-non-private-member-variables-in-classes)
    OnEventTypeList on_event_types; //NOLINT(misc-non-private-member-variables-in-classes)
    bool has_on_exit_any = false; //NOLINT(misc-non-private-member-variables-in-classes)
    bool has_pretty_name_fn = false; //NOLINT(misc-non-private-member-variables-in-classes)

    [[nodiscard]] constexpr auto on_entry_any() const
    {
        return detail::make_state_conf
        (
            true,
            on_event_auto,
            on_event_types,
            has_on_exit_any,
            has_pretty_name_fn
        );
    }

    [[nodiscard]] constexpr auto enable_on_event_auto() const
    {
        return detail::make_state_conf
        (
            has_on_entry_any,
            true,
            on_event_types,
            has_on_exit_any,
            has_pretty_name_fn
        );
    }

    template<class... Types>
    [[nodiscard]] constexpr auto on_event() const
    {
        return detail::make_state_conf
        (
            has_on_entry_any,
            on_event_auto,
            detail::type_list_c<Types...>,
            has_on_exit_any,
            has_pretty_name_fn
        );
    }

    [[nodiscard]] constexpr auto on_exit_any() const
    {
        return detail::make_state_conf
        (
            has_on_entry_any,
            on_event_auto,
            on_event_types,
            true,
            has_pretty_name_fn
        );
    }

    [[nodiscard]] constexpr auto pretty_name_fn() const
    {
        return detail::make_state_conf
        (
            has_on_entry_any,
            on_event_auto,
            on_event_types,
            has_on_exit_any,
            true
        );
    }
};

inline constexpr auto state_conf_c = state_conf<>{};

namespace detail
{
    template<class... Args>
    constexpr auto make_state_conf(const Args&... args)
    {
        using args_t = detail::type_list<Args...>;
        using on_event_type_list = tlu::get_t<args_t, 2>;
        return state_conf<on_event_type_list>{args...};
    }
}

} //namespace

#endif
