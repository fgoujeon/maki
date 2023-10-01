//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_STATE_CONF_HPP
#define MAKI_STATE_CONF_HPP

#include "type_list.hpp"
#include "type.hpp"
#include "detail/tlu.hpp"

namespace maki
{

namespace detail
{
    template<class... Args>
    constexpr auto make_state_conf(const Args&... args);
}

template<class OnEventTypeList = type_list<>>
struct state_conf
{
    bool on_entry = false; //NOLINT(misc-non-private-member-variables-in-classes)
    OnEventTypeList on_event; //NOLINT(misc-non-private-member-variables-in-classes)
    bool on_event_auto = false; //NOLINT(misc-non-private-member-variables-in-classes)
    bool on_exit = false; //NOLINT(misc-non-private-member-variables-in-classes)
    bool pretty_name_fn = false; //NOLINT(misc-non-private-member-variables-in-classes)

#define MAKI_DETAIL_MAKE_STATE_CONF_COPY(changed_var_name, new_value) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    [[maybe_unused]] const auto arg_on_entry = on_entry; \
    [[maybe_unused]] const auto arg_on_event = on_event; \
    [[maybe_unused]] const auto arg_on_event_auto = on_event_auto; \
    [[maybe_unused]] const auto arg_on_exit = on_exit; \
    [[maybe_unused]] const auto arg_pretty_name_fn = pretty_name_fn; \
 \
    { \
        const auto arg_##changed_var_name = new_value; \
 \
        return state_conf \
        < \
            std::decay_t<decltype(arg_on_event)> \
        > \
        { \
            arg_on_entry, \
            arg_on_event, \
            arg_on_event_auto, \
            arg_on_exit, \
            arg_pretty_name_fn \
        }; \
    }

    [[nodiscard]] constexpr auto enable_on_entry() const
    {
        MAKI_DETAIL_MAKE_STATE_CONF_COPY(on_entry, true)
    }

    [[nodiscard]] constexpr auto enable_on_event_auto() const
    {
        MAKI_DETAIL_MAKE_STATE_CONF_COPY(on_event_auto, true)
    }

    template<class... Types>
    [[nodiscard]] constexpr auto enable_on_event() const
    {
        MAKI_DETAIL_MAKE_STATE_CONF_COPY(on_event, type_list_c<Types...>)
    }

    template<class... Types>
    [[nodiscard]] constexpr auto enable_on_event(const type_list<Types...> /*value*/) const
    {
        MAKI_DETAIL_MAKE_STATE_CONF_COPY(on_event, type_list_c<Types...>)
    }

    [[nodiscard]] constexpr auto enable_on_exit() const
    {
        MAKI_DETAIL_MAKE_STATE_CONF_COPY(on_exit, true)
    }

    [[nodiscard]] constexpr auto enable_pretty_name() const
    {
        MAKI_DETAIL_MAKE_STATE_CONF_COPY(pretty_name_fn, true)
    }

#undef MAKI_DETAIL_MAKE_STATE_CONF_COPY
};

inline constexpr auto state_conf_c = state_conf<>{};

namespace detail
{
    template<class... Args>
    constexpr auto make_state_conf(const Args&... args)
    {
        using args_t = type_list<Args...>;
        using on_event_type_list = tlu::get_t<args_t, 2>;
        return state_conf<on_event_type_list>{args...};
    }
}

} //namespace

#endif
