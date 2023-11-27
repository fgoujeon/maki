//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::submachine_conf struct template
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

namespace maki
{

/**
@brief Submachine configuration
*/
template
<
    class Data = void,
    class ContextTypeHolder = type<void>,
    class EntryActionTuple = detail::tuple<>,
    class EventActionTuple = detail::tuple<>,
    class ExitActionTuple = detail::tuple<>,
    class TransitionTableTypeList = type_list<>
>
struct submachine_conf
{
    using data_type = Data;
    using context_type = typename ContextTypeHolder::type;

    ContextTypeHolder context; //NOLINT(misc-non-private-member-variables-in-classes)
    EntryActionTuple entry_actions; //NOLINT(misc-non-private-member-variables-in-classes)
    EventActionTuple event_actions; //NOLINT(misc-non-private-member-variables-in-classes)
    ExitActionTuple exit_actions; //NOLINT(misc-non-private-member-variables-in-classes)
    bool has_pretty_name = false; //NOLINT(misc-non-private-member-variables-in-classes)
    TransitionTableTypeList transition_tables; //NOLINT(misc-non-private-member-variables-in-classes)

#define MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_BEGIN /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_context = context; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_entry_actions = entry_actions; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_event_actions = event_actions; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_exit_actions = exit_actions; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_has_pretty_name = has_pretty_name; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_transition_tables = transition_tables;

#define MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_END /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    return submachine_conf \
    < \
        Data, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_context)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_entry_actions)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_event_actions)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_exit_actions)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_transition_tables)> \
    > \
    { \
        MAKI_DETAIL_ARG_context, \
        MAKI_DETAIL_ARG_entry_actions, \
        MAKI_DETAIL_ARG_event_actions, \
        MAKI_DETAIL_ARG_exit_actions, \
        MAKI_DETAIL_ARG_has_pretty_name, \
        MAKI_DETAIL_ARG_transition_tables \
    };

    template<class EventFilter, detail::event_action_signature Sig, class Action>
    [[nodiscard]] constexpr auto entry_action(const Action& action) const
    {
        const auto new_entry_actions = tuple_append
        (
            entry_actions,
            detail::event_action<EventFilter, Action, Sig>{action}
        );

        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_entry_actions new_entry_actions
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_entry_actions
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto entry_action_v(const Action& action) const
    {
        return entry_action<EventFilter, detail::event_action_signature::v>(action);
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto entry_action_m(const Action& action) const
    {
        return entry_action<EventFilter, detail::event_action_signature::m>(action);
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto entry_action_c(const Action& action) const
    {
        return entry_action<EventFilter, detail::event_action_signature::c>(action);
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto entry_action_ce(const Action& action) const
    {
        return entry_action<EventFilter, detail::event_action_signature::ce>(action);
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto entry_action_d(const Action& action) const
    {
        return entry_action<EventFilter, detail::event_action_signature::d>(action);
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto entry_action_de(const Action& action) const
    {
        return entry_action<EventFilter, detail::event_action_signature::de>(action);
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto entry_action_e(const Action& action) const
    {
        return entry_action<EventFilter, detail::event_action_signature::e>(action);
    }

    template<class EventFilter, detail::event_action_signature Sig, class Action>
    [[nodiscard]] constexpr auto event_action(const Action& action) const
    {
        const auto new_event_actions = tuple_append
        (
            event_actions,
            detail::event_action<EventFilter, Action, Sig>{action}
        );

        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_event_actions new_event_actions
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_event_actions
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto event_action_v(const Action& action) const
    {
        return event_action<EventFilter, detail::event_action_signature::v>(action);
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto event_action_m(const Action& action) const
    {
        return event_action<EventFilter, detail::event_action_signature::m>(action);
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto event_action_c(const Action& action) const
    {
        return event_action<EventFilter, detail::event_action_signature::c>(action);
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto event_action_ce(const Action& action) const
    {
        return event_action<EventFilter, detail::event_action_signature::ce>(action);
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto event_action_d(const Action& action) const
    {
        return event_action<EventFilter, detail::event_action_signature::d>(action);
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto event_action_de(const Action& action) const
    {
        return event_action<EventFilter, detail::event_action_signature::de>(action);
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto event_action_e(const Action& action) const
    {
        return event_action<EventFilter, detail::event_action_signature::e>(action);
    }

    template<class EventFilter, detail::event_action_signature Sig, class Action>
    [[nodiscard]] constexpr auto exit_action(const Action& action) const
    {
        const auto new_exit_actions = tuple_append
        (
            exit_actions,
            detail::event_action<EventFilter, Action, Sig>{action}
        );

        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_exit_actions new_exit_actions
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_exit_actions
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto exit_action_v(const Action& action) const
    {
        return exit_action<EventFilter, detail::event_action_signature::v>(action);
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto exit_action_m(const Action& action) const
    {
        return exit_action<EventFilter, detail::event_action_signature::m>(action);
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto exit_action_c(const Action& action) const
    {
        return exit_action<EventFilter, detail::event_action_signature::c>(action);
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto exit_action_ce(const Action& action) const
    {
        return exit_action<EventFilter, detail::event_action_signature::ce>(action);
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto exit_action_d(const Action& action) const
    {
        return exit_action<EventFilter, detail::event_action_signature::d>(action);
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto exit_action_de(const Action& action) const
    {
        return exit_action<EventFilter, detail::event_action_signature::de>(action);
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto exit_action_e(const Action& action) const
    {
        return exit_action<EventFilter, detail::event_action_signature::e>(action);
    }

    template<class Context>
    [[nodiscard]] constexpr auto set_context() const
    {
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_context type_c<Context>
        MAKI_DETAIL_MAKE_SUBMACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_context
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

template<class Data = void>
constexpr auto submachine_conf_c = submachine_conf<Data>{};

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
