//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::submachine_conf_t struct template
*/

#ifndef MAKI_SUBMACHINE_CONF_HPP
#define MAKI_SUBMACHINE_CONF_HPP

#include "transition_table.hpp"
#include "type_patterns.hpp"
#include "type_list.hpp"
#include "type.hpp"
#include "detail/event_action.hpp"
#include "detail/tuple.hpp"
#include "detail/tlu.hpp"
#include <string_view>

namespace maki
{

#ifdef MAKI_DETAIL_DOXYGEN
/**
@brief Submachine configuration
*/
template<IMPLEMENTATION_DETAIL>
#else
template
<
    class Data = void,
    class Context = void,
    class EntryActionTuple = detail::tuple<>,
    class InternalActionTuple = detail::tuple<>,
    class ExitActionTuple = detail::tuple<>,
    class TransitionTableTypeList = type_list<>
>
#endif
class submachine_conf_t
{
public:
    using data_type = Data;
    using context_type = Context;

#define MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_BEGIN /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_data_type = type_c<data_type>; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_context_type = type_c<context_type>; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_entry_actions = entry_actions_; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_internal_actions = internal_actions_; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_exit_actions = exit_actions_; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_pretty_name_view = pretty_name_; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_transition_tables = transition_tables_;

#define MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_END /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    return submachine_conf_t \
    < \
        typename std::decay_t<decltype(MAKI_DETAIL_ARG_data_type)>::type, \
        typename std::decay_t<decltype(MAKI_DETAIL_ARG_context_type)>::type, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_entry_actions)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_internal_actions)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_exit_actions)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_transition_tables)> \
    > \
    { \
        MAKI_DETAIL_ARG_entry_actions, \
        MAKI_DETAIL_ARG_internal_actions, \
        MAKI_DETAIL_ARG_exit_actions, \
        MAKI_DETAIL_ARG_pretty_name_view, \
        MAKI_DETAIL_ARG_transition_tables \
    };

    template<class Context2>
    [[nodiscard]] constexpr auto context() const
    {
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_context_type type_c<Context2>
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_context_type
    }

    template<class Data2>
    [[nodiscard]] constexpr auto data() const
    {
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_data_type type_c<Data2>
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_data_type
    }

#define MAKI_DETAIL_X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    template<class EventFilter = maki::any_t, class Action> \
    [[nodiscard]] constexpr auto entry_action_##signature(const Action& action) const \
    { \
        return entry_action<EventFilter, detail::event_action_signature::signature>(action); \
    }
    MAKI_DETAIL_EVENT_ACTION_SIGNATURES
#undef MAKI_DETAIL_X

#define MAKI_DETAIL_X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    template<class EventFilter, class Action> \
    [[nodiscard]] constexpr auto internal_action_##signature(const Action& action) const \
    { \
        return internal_action<EventFilter, detail::event_action_signature::signature>(action); \
    }
    MAKI_DETAIL_EVENT_ACTION_SIGNATURES
#undef MAKI_DETAIL_X

#define MAKI_DETAIL_X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    template<class EventFilter = maki::any_t, class Action> \
    [[nodiscard]] constexpr auto exit_action_##signature(const Action& action) const \
    { \
        return exit_action<EventFilter, detail::event_action_signature::signature>(action); \
    }
    MAKI_DETAIL_EVENT_ACTION_SIGNATURES
#undef MAKI_DETAIL_X

    [[nodiscard]] constexpr auto pretty_name(const std::string_view value) const
    {
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_pretty_name_view value
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_pretty_name_view
    }

    template<class... TransitionTables>
    [[nodiscard]] constexpr auto transition_tables(const TransitionTables&... /*tables*/) const
    {
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_transition_tables type_list_c<TransitionTables...>
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_transition_tables
    }

#if MAKI_DETAIL_DOXYGEN
private:
#endif
    template<class EventFilter = maki::any_t, detail::event_action_signature Sig, class Action>
    [[nodiscard]] constexpr auto entry_action(const Action& action) const
    {
        const auto new_entry_actions = tuple_append
        (
            entry_actions_,
            detail::event_action<EventFilter, Action, Sig>{action}
        );

        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_entry_actions new_entry_actions
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_entry_actions
    }

    template<class EventFilter, detail::event_action_signature Sig, class Action>
    [[nodiscard]] constexpr auto internal_action(const Action& action) const
    {
        const auto new_internal_actions = tuple_append
        (
            internal_actions_,
            detail::event_action<EventFilter, Action, Sig>{action}
        );

        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_internal_actions new_internal_actions
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_internal_actions
    }

    template<class EventFilter = maki::any_t, detail::event_action_signature Sig, class Action>
    [[nodiscard]] constexpr auto exit_action(const Action& action) const
    {
        const auto new_exit_actions = tuple_append
        (
            exit_actions_,
            detail::event_action<EventFilter, Action, Sig>{action}
        );

        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_exit_actions new_exit_actions
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_exit_actions
    }

#undef MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_END
#undef MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_BEGIN

    EntryActionTuple entry_actions_; //NOLINT(misc-non-private-member-variables-in-classes)
    InternalActionTuple internal_actions_; //NOLINT(misc-non-private-member-variables-in-classes)
    ExitActionTuple exit_actions_; //NOLINT(misc-non-private-member-variables-in-classes)
    std::string_view pretty_name_; //NOLINT(misc-non-private-member-variables-in-classes)
    TransitionTableTypeList transition_tables_; //NOLINT(misc-non-private-member-variables-in-classes)
};

inline constexpr auto submachine_conf = submachine_conf_t{};

namespace detail
{
    template<class T>
    struct is_submachine_conf
    {
        static constexpr auto value = false;
    };

    template<class... Ts>
    struct is_submachine_conf<submachine_conf_t<Ts...>>
    {
        static constexpr auto value = true;
    };

    template<class T>
    constexpr auto is_submachine_conf_v = is_submachine_conf<T>::value;
}

} //namespace

#endif
