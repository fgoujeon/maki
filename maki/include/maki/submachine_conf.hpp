//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_SUBMACHINE_CONF_HPP
#define MAKI_SUBMACHINE_CONF_HPP

#include "transition_table.hpp"
#include "type_patterns.hpp"
#include "detail/type_list.hpp"
#include "detail/type.hpp"
#include "detail/tlu.hpp"

namespace maki
{

namespace detail
{
    template<class... Args>
    constexpr auto make_submachine_conf(const Args&... args);
}

template
<
    class ContextType = detail::type_t<void>,
    class OnEventTypeList = detail::type_list<>,
    class TransitionTableTypeList = detail::type_list<>
>
struct submachine_conf
{
    ContextType context_type; //NOLINT(misc-non-private-member-variables-in-classes)
    bool has_on_entry_any = false; //NOLINT(misc-non-private-member-variables-in-classes)
    OnEventTypeList on_event_types; //NOLINT(misc-non-private-member-variables-in-classes)
    bool on_event_auto = false; //NOLINT(misc-non-private-member-variables-in-classes)
    bool has_on_exit_any = false; //NOLINT(misc-non-private-member-variables-in-classes)
    bool has_pretty_name_fn = false; //NOLINT(misc-non-private-member-variables-in-classes)
    TransitionTableTypeList transition_table_types; //NOLINT(misc-non-private-member-variables-in-classes)

#define MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY(changed_var_name, new_value) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    [[maybe_unused]] const auto arg_context_type = context_type; \
    [[maybe_unused]] const auto arg_has_on_entry_any = has_on_entry_any; \
    [[maybe_unused]] const auto arg_on_event_types = on_event_types; \
    [[maybe_unused]] const auto arg_on_event_auto = on_event_auto; \
    [[maybe_unused]] const auto arg_has_on_exit_any = has_on_exit_any; \
    [[maybe_unused]] const auto arg_has_pretty_name_fn = has_pretty_name_fn; \
    [[maybe_unused]] const auto arg_transition_table_types = transition_table_types; \
 \
    { \
        const auto arg_##changed_var_name = new_value; \
 \
        return detail::make_submachine_conf \
        ( \
            arg_context_type, \
            arg_has_on_entry_any, \
            arg_on_event_types, \
            arg_on_event_auto, \
            arg_has_on_exit_any, \
            arg_has_pretty_name_fn, \
            arg_transition_table_types \
        ); \
    }

    template<class Context>
    [[nodiscard]] constexpr auto set_context_type() const
    {
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY(context_type, detail::type_c<Context>)
    }

    [[nodiscard]] constexpr auto on_entry_any() const
    {
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY(has_on_entry_any, true)
    }

    [[nodiscard]] constexpr auto enable_on_event_auto() const
    {
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY(on_event_auto, true)
    }

    template<class... Types>
    [[nodiscard]] constexpr auto on_event() const
    {
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY(on_event_types, detail::type_list<Types...>{})
    }

    [[nodiscard]] constexpr auto on_exit_any() const
    {
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY(has_on_exit_any, true)
    }

    [[nodiscard]] constexpr auto pretty_name_fn() const
    {
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY(has_pretty_name_fn, true)
    }

    template<class... Ts>
    [[nodiscard]] constexpr auto set_transition_tables() const
    {
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY(transition_table_types, detail::type_list<Ts...>{})
    }

#undef MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY
};

inline constexpr auto submachine_conf_c = submachine_conf<>{};

namespace detail
{
    template<class... Args>
    constexpr auto make_submachine_conf(const Args&... args)
    {
        using args_t = detail::type_list<Args...>;
        constexpr auto context_type_arg_index = 0;
        constexpr auto on_event_type_list_arg_index = 2;
        constexpr auto transition_table_list_type_index = 6;
        return submachine_conf
        <
            tlu::get_t<args_t, context_type_arg_index>,
            tlu::get_t<args_t, on_event_type_list_arg_index>,
            tlu::get_t<args_t, transition_table_list_type_index>
        >{args...};
    }

    template<class T>
    struct is_submachine_conf
    {
        static constexpr auto value = false;
    };

    template<class... Ts>
    struct is_submachine_conf<submachine_conf<Ts...>>
    {
        static constexpr auto value = true;
    };

    template<class T>
    constexpr auto is_submachine_conf_v = is_submachine_conf<T>::value;
}

} //namespace

#endif
