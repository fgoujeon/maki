//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_SUBMACHINE_CONF_HPP
#define MAKI_SUBMACHINE_CONF_HPP

#include "transition_table.hpp"
#include "type_patterns.hpp"
#include "type_list.hpp"
#include "type.hpp"
#include "detail/tlu.hpp"

namespace maki
{

template
<
    class ContextType = type<void>,
    class OnEventTypeList = type_list<>,
    class TransitionTableTypeList = type_list<>
>
struct submachine_conf
{
    ContextType context_type; //NOLINT(misc-non-private-member-variables-in-classes)
    bool has_on_entry = false; //NOLINT(misc-non-private-member-variables-in-classes)
    bool has_on_event_auto = false; //NOLINT(misc-non-private-member-variables-in-classes)
    OnEventTypeList has_on_event_for; //NOLINT(misc-non-private-member-variables-in-classes)
    bool has_on_exit = false; //NOLINT(misc-non-private-member-variables-in-classes)
    bool has_pretty_name = false; //NOLINT(misc-non-private-member-variables-in-classes)
    TransitionTableTypeList transition_tables; //NOLINT(misc-non-private-member-variables-in-classes)

#define MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_BEGIN /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_context_type = context_type; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_has_on_entry = has_on_entry; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_has_on_event_auto = has_on_event_auto; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_has_on_event_for = has_on_event_for; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_has_on_exit = has_on_exit; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_has_pretty_name = has_pretty_name; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_transition_tables = transition_tables;

#define MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_END /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    return submachine_conf \
    < \
        std::decay_t<decltype(MAKI_DETAIL_ARG_context_type)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_has_on_event_for)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_transition_tables)> \
    > \
    { \
        MAKI_DETAIL_ARG_context_type, \
        MAKI_DETAIL_ARG_has_on_entry, \
        MAKI_DETAIL_ARG_has_on_event_auto, \
        MAKI_DETAIL_ARG_has_on_event_for, \
        MAKI_DETAIL_ARG_has_on_exit, \
        MAKI_DETAIL_ARG_has_pretty_name, \
        MAKI_DETAIL_ARG_transition_tables \
    };

    template<class Context>
    [[nodiscard]] constexpr auto set_context_type() const
    {
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_context_type type_c<Context>
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_context_type
    }

    [[nodiscard]] constexpr auto enable_on_entry() const
    {
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_has_on_entry true
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_has_on_entry
    }

    [[nodiscard]] constexpr auto enable_on_event_auto() const
    {
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_has_on_event_auto true
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_has_on_event_auto
    }

    template<class... Types>
    [[nodiscard]] constexpr auto enable_on_event_for() const
    {
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_has_on_event_for type_list_c<Types...>
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_has_on_event_for
    }

    [[nodiscard]] constexpr auto enable_on_exit() const
    {
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_has_on_exit true
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_has_on_exit
    }

    [[nodiscard]] constexpr auto enable_pretty_name() const
    {
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_has_pretty_name true
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_has_pretty_name
    }

    template<class... TransitionTables>
    [[nodiscard]] constexpr auto set_transition_tables(const TransitionTables&... /*tables*/) const
    {
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_transition_tables type_list_c<TransitionTables...>
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_transition_tables
    }

#undef MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_END
#undef MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_BEGIN
};

inline constexpr auto default_submachine_conf = submachine_conf<>{};

namespace detail
{
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
