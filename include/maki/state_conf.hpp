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

#include "type_patterns.hpp"
#include "type.hpp"
#include "detail/event_action.hpp"
#include "detail/tuple.hpp"
#include "detail/tlu.hpp"
#include <string_view>

namespace maki
{

namespace detail
{
    template
    <
        class Context = void,
        class EntryActionTuple = detail::tuple<>,
        class InternalActionTuple = detail::tuple<>,
        class ExitActionTuple = detail::tuple<>
    >
    struct state_conf_option_set
    {
        using context_type = Context;

        EntryActionTuple entry_actions;
        InternalActionTuple internal_actions;
        ExitActionTuple exit_actions;
        std::string_view pretty_name;
    };
}

#ifdef MAKI_DETAIL_DOXYGEN
/**
@brief State configuration
*/
template<IMPLEMENTATION_DETAIL>
#else
template<class OptionSet = detail::state_conf_option_set<>>
#endif
class state_conf;

namespace detail
{
    //Read access to options
    template<class OptionSet>
    constexpr const auto& opts(const state_conf<OptionSet>& conf)
    {
        return conf.options_;
    }
}

#ifdef MAKI_DETAIL_DOXYGEN
template<IMPLEMENTATION_DETAIL>
#else
template<class OptionSet>
#endif
class state_conf
{
public:
    constexpr state_conf() = default;

    state_conf(const state_conf&) = delete;

    state_conf(state_conf&&) = delete;

    ~state_conf() = default;

    state_conf& operator=(const state_conf&) = delete;

    state_conf& operator=(state_conf&&) = delete;

    constexpr bool operator==(const state_conf& other) const
    {
        return this == &other;
    }

#define MAKI_DETAIL_MAKE_STATE_CONF_COPY_BEGIN /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_context_type = type_c<typename OptionSet::context_type>; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_entry_actions = options_.entry_actions; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_internal_actions = options_.internal_actions; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_exit_actions = options_.exit_actions; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_pretty_name_view = options_.pretty_name;

#define MAKI_DETAIL_MAKE_STATE_CONF_COPY_END /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    return state_conf \
    < \
        detail::state_conf_option_set \
        < \
            typename std::decay_t<decltype(MAKI_DETAIL_ARG_context_type)>::type, \
            std::decay_t<decltype(MAKI_DETAIL_ARG_entry_actions)>, \
            std::decay_t<decltype(MAKI_DETAIL_ARG_internal_actions)>, \
            std::decay_t<decltype(MAKI_DETAIL_ARG_exit_actions)> \
        > \
    > \
    { \
        MAKI_DETAIL_ARG_entry_actions, \
        MAKI_DETAIL_ARG_internal_actions, \
        MAKI_DETAIL_ARG_exit_actions, \
        MAKI_DETAIL_ARG_pretty_name_view \
    };

    template<class Context2>
    [[nodiscard]] constexpr auto context() const
    {
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_context_type type_c<Context2>
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_context_type
    }

#define MAKI_DETAIL_X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    template<const auto& EventFilter = maki::any, class Action> \
    [[nodiscard]] constexpr auto entry_action_##signature(const Action& action) const \
    { \
        return entry_action<EventFilter, detail::event_action_signature::signature>(action); \
    }
    MAKI_DETAIL_EVENT_ACTION_SIGNATURES
#undef MAKI_DETAIL_X

#define MAKI_DETAIL_X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    template<const auto& EventFilter, class Action> \
    [[nodiscard]] constexpr auto internal_action_##signature(const Action& action) const \
    { \
        return internal_action<EventFilter, detail::event_action_signature::signature>(action); \
    }
    MAKI_DETAIL_EVENT_ACTION_SIGNATURES
#undef MAKI_DETAIL_X

#define MAKI_DETAIL_X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    template<const auto& EventFilter = maki::any, class Action> \
    [[nodiscard]] constexpr auto exit_action_##signature(const Action& action) const \
    { \
        return exit_action<EventFilter, detail::event_action_signature::signature>(action); \
    }
    MAKI_DETAIL_EVENT_ACTION_SIGNATURES
#undef MAKI_DETAIL_X

    [[nodiscard]] constexpr auto pretty_name(const std::string_view value) const
    {
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_pretty_name_view value
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_pretty_name_view
    }

private:
    template<class Storage2>
    friend class state_conf;

    template<class Storage2>
    friend constexpr const auto& detail::opts(const state_conf<Storage2>& conf);

    template<class... Args>
    constexpr state_conf(Args&&... args):
        options_{std::forward<Args>(args)...}
    {
    }

    template<const auto& EventFilter = maki::any, detail::event_action_signature Sig, class Action>
    [[nodiscard]] constexpr auto entry_action(const Action& action) const
    {
        const auto new_entry_actions = tuple_append
        (
            options_.entry_actions,
            detail::make_event_action<Sig>(EventFilter, action)
        );

        MAKI_DETAIL_MAKE_STATE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_entry_actions new_entry_actions
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_entry_actions
    }

    template<const auto& EventFilter, detail::event_action_signature Sig, class Action>
    [[nodiscard]] constexpr auto internal_action(const Action& action) const
    {
        const auto new_internal_actions = tuple_append
        (
            options_.internal_actions,
            detail::make_event_action<Sig>(EventFilter, action)
        );

        MAKI_DETAIL_MAKE_STATE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_internal_actions new_internal_actions
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_internal_actions
    }

    template<const auto& EventFilter = maki::any, detail::event_action_signature Sig, class Action>
    [[nodiscard]] constexpr auto exit_action(const Action& action) const
    {
        const auto new_exit_actions = tuple_append
        (
            options_.exit_actions,
            detail::make_event_action<Sig>(EventFilter, action)
        );

        MAKI_DETAIL_MAKE_STATE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_exit_actions new_exit_actions
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_exit_actions
    }

#undef MAKI_DETAIL_MAKE_STATE_CONF_COPY_END
#undef MAKI_DETAIL_MAKE_STATE_CONF_COPY_BEGIN

    OptionSet options_;
};

} //namespace

#endif
