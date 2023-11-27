//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::state_conf struct template
*/

#ifndef MAKI_STATE_CONF_HPP
#define MAKI_STATE_CONF_HPP

#include "type_list.hpp"
#include "type.hpp"
#include "detail/event_action.hpp"
#include "detail/tuple.hpp"
#include "detail/tlu.hpp"

namespace maki
{

namespace detail
{
    template<class... Args>
    constexpr auto make_state_conf(const Args&... args);
}

/**
@brief State configuration
*/
template
<
    class EntryActionTuple = detail::tuple<>,
    class EventActionTuple = detail::tuple<>,
    class ExitActionTuple = detail::tuple<>
>
struct state_conf
{
    EntryActionTuple entry_actions; //NOLINT(misc-non-private-member-variables-in-classes)
    EventActionTuple event_actions; //NOLINT(misc-non-private-member-variables-in-classes)
    ExitActionTuple exit_actions; //NOLINT(misc-non-private-member-variables-in-classes)
    bool has_pretty_name = false; //NOLINT(misc-non-private-member-variables-in-classes)

#define MAKI_DETAIL_MAKE_STATE_CONF_COPY_BEGIN /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_entry_actions = entry_actions; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_event_actions = event_actions; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_exit_actions = exit_actions; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_has_pretty_name = has_pretty_name;

#define MAKI_DETAIL_MAKE_STATE_CONF_COPY_END /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    return state_conf \
    < \
        std::decay_t<decltype(MAKI_DETAIL_ARG_entry_actions)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_event_actions)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_exit_actions)> \
    > \
    { \
        MAKI_DETAIL_ARG_entry_actions, \
        MAKI_DETAIL_ARG_event_actions, \
        MAKI_DETAIL_ARG_exit_actions, \
        MAKI_DETAIL_ARG_has_pretty_name \
    };

    template<class EventFilter, detail::event_action_signature Sig, class Action>
    [[nodiscard]] constexpr auto entry_action(const Action& action) const
    {
        const auto new_entry_actions = tuple_append
        (
            entry_actions,
            detail::event_action<EventFilter, Action, Sig>{action}
        );

        MAKI_DETAIL_MAKE_STATE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_entry_actions new_entry_actions
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_END
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

        MAKI_DETAIL_MAKE_STATE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_event_actions new_event_actions
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_END
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
    [[nodiscard]] constexpr auto event_action_mce(const Action& action) const
    {
        return event_action<EventFilter, detail::event_action_signature::mce>(action);
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

        MAKI_DETAIL_MAKE_STATE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_exit_actions new_exit_actions
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_END
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

    [[nodiscard]] constexpr auto enable_pretty_name() const
    {
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_has_pretty_name true
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_has_pretty_name
    }

#undef MAKI_DETAIL_MAKE_STATE_CONF_COPY_END
#undef MAKI_DETAIL_MAKE_STATE_CONF_COPY_BEGIN
};

inline constexpr auto default_state_conf = state_conf<>{};

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
