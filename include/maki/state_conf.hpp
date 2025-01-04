//Copyright Florian Goujeon 2021 - 2025.
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

#include "state_conf_fwd.hpp"
#include "action.hpp"
#include "filters.hpp"
#include "type.hpp"
#include "detail/context_signature.hpp"
#include "detail/event_action.hpp"
#include "detail/signature_macros.hpp"
#include "detail/tuple.hpp"
#include "detail/tlu.hpp"
#include <string_view>
#include <type_traits>

namespace maki
{

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
#define MAKI_DETAIL_STATE_CONF_RETURN_TYPE state_conf<IMPLEMENTATION_DETAIL>
#else
#define MAKI_DETAIL_STATE_CONF_RETURN_TYPE auto
#endif

#ifdef MAKI_DETAIL_DOXYGEN
template<IMPLEMENTATION_DETAIL>
#else
template<class OptionSet>
#endif
class state_conf
{
public:
    using context_type = typename OptionSet::context_type;

    constexpr state_conf() = default;

    state_conf(const state_conf&) = delete;

    state_conf(state_conf&&) = delete;

    ~state_conf() = default;

    state_conf& operator=(const state_conf&) = delete;

    state_conf& operator=(state_conf&&) = delete;

#define MAKI_DETAIL_MAKE_STATE_CONF_COPY_BEGIN /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_context_type = type<typename OptionSet::context_type>; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_context_sig = options_.context_sig; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_entry_actions = options_.entry_actions; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_internal_actions = options_.internal_actions; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_exit_actions = options_.exit_actions; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_pretty_name_view = options_.pretty_name; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_transition_tables = options_.transition_tables;

#define MAKI_DETAIL_MAKE_STATE_CONF_COPY_END /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    return state_conf \
    < \
        detail::state_conf_option_set \
        < \
            typename std::decay_t<decltype(MAKI_DETAIL_ARG_context_type)>::type, \
            std::decay_t<decltype(MAKI_DETAIL_ARG_entry_actions)>, \
            std::decay_t<decltype(MAKI_DETAIL_ARG_internal_actions)>, \
            std::decay_t<decltype(MAKI_DETAIL_ARG_exit_actions)>, \
            std::decay_t<decltype(MAKI_DETAIL_ARG_transition_tables)> \
        > \
    > \
    { \
        MAKI_DETAIL_ARG_context_sig, \
        MAKI_DETAIL_ARG_entry_actions, \
        MAKI_DETAIL_ARG_internal_actions, \
        MAKI_DETAIL_ARG_exit_actions, \
        MAKI_DETAIL_ARG_pretty_name_view, \
        MAKI_DETAIL_ARG_transition_tables \
    };

#define MAKI_DETAIL_X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    /** \
    @brief Sets the type of the context. \
    */ \
    template<class Context> \
    [[nodiscard]] constexpr MAKI_DETAIL_STATE_CONF_RETURN_TYPE context_##signature() const \
    { \
        return context<Context, detail::context_signature::signature>(); \
    }
    MAKI_DETAIL_STATE_CONTEXT_CONSTRUCTOR_SIGNATURES
#undef MAKI_DETAIL_X

#define MAKI_DETAIL_X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    /** \
    @brief Add an entry action. \
    */ \
    template<class EventFilter, class Action> \
    [[nodiscard]] constexpr MAKI_DETAIL_STATE_CONF_RETURN_TYPE entry_action_##signature(const EventFilter& event_filter, const Action& action) const \
    { \
        return entry_action<action_signature::signature>(event_filter, action); \
    }
    MAKI_DETAIL_ACTION_SIGNATURES
#undef MAKI_DETAIL_X

#define MAKI_DETAIL_X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    /** \
    @brief Add an entry action. \
    */ \
    template<class Action> \
    [[nodiscard]] constexpr MAKI_DETAIL_STATE_CONF_RETURN_TYPE entry_action_##signature(const Action& action) const \
    { \
        return entry_action<action_signature::signature>(any, action); \
    }
    MAKI_DETAIL_ACTION_SIGNATURES
#undef MAKI_DETAIL_X

#define MAKI_DETAIL_X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    /** \
    @brief Add an internal action. \
    */ \
    template<class EventFilter, class Action> \
    [[nodiscard]] constexpr MAKI_DETAIL_STATE_CONF_RETURN_TYPE internal_action_##signature(const EventFilter& event_filter, const Action& action) const \
    { \
        return internal_action<action_signature::signature>(event_filter, action); \
    }
    MAKI_DETAIL_ACTION_SIGNATURES
#undef MAKI_DETAIL_X

#define MAKI_DETAIL_X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    /** \
    @brief Add an exit action. \
    */ \
    template<class EventFilter, class Action> \
    [[nodiscard]] constexpr MAKI_DETAIL_STATE_CONF_RETURN_TYPE exit_action_##signature(const EventFilter& event_filter, const Action& action) const \
    { \
        return exit_action<action_signature::signature>(event_filter, action); \
    }
    MAKI_DETAIL_ACTION_SIGNATURES
#undef MAKI_DETAIL_X

#define MAKI_DETAIL_X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    /** \
    @brief Add an exit action. \
    */ \
    template<class Action> \
    [[nodiscard]] constexpr MAKI_DETAIL_STATE_CONF_RETURN_TYPE exit_action_##signature(const Action& action) const \
    { \
        return exit_action<action_signature::signature>(any, action); \
    }
    MAKI_DETAIL_ACTION_SIGNATURES
#undef MAKI_DETAIL_X

    /**
    @brief Specifies the pretty name of the state.

    See `maki::pretty_name()`.
    */
    [[nodiscard]] constexpr MAKI_DETAIL_STATE_CONF_RETURN_TYPE pretty_name(const std::string_view value) const
    {
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_pretty_name_view value
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_pretty_name_view
    }

    /**
    @brief Specifies the list of transition tables. One region per transition
    table is created.
    */
    template<class... TransitionTables>
    [[nodiscard]] constexpr MAKI_DETAIL_STATE_CONF_RETURN_TYPE transition_tables(const TransitionTables&... tables) const
    {
        const auto tpl = detail::tuple<TransitionTables...>{detail::distributed_construct, tables...};
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_transition_tables tpl
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_transition_tables
    }

private:
    template<class OptionSet2>
    friend class state_conf;

    template<class OptionSet2>
    friend constexpr const auto& detail::opts(const state_conf<OptionSet2>& conf);

    template<class... Args>
    constexpr state_conf(Args&&... args):
        options_{std::forward<Args>(args)...}
    {
    }

    template<class Context, auto ContextSig>
    [[nodiscard]] constexpr auto context() const
    {
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_context_type type<Context>
#define MAKI_DETAIL_ARG_context_sig ContextSig
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_context_type
#undef MAKI_DETAIL_ARG_context_sig
    }

    template<action_signature Sig, class EventFilter, class Action>
    [[nodiscard]] constexpr auto entry_action(const EventFilter& event_filter, const Action& action) const
    {
        const auto new_entry_actions = tuple_append
        (
            options_.entry_actions,
            detail::make_event_action<Sig>(event_filter, action)
        );

        MAKI_DETAIL_MAKE_STATE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_entry_actions new_entry_actions
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_entry_actions
    }

    template<action_signature Sig, class EventFilter, class Action>
    [[nodiscard]] constexpr auto internal_action(const EventFilter& event_filter, const Action& action) const
    {
        const auto new_internal_actions = tuple_append
        (
            options_.internal_actions,
            detail::make_event_action<Sig>(event_filter, action)
        );

        MAKI_DETAIL_MAKE_STATE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_internal_actions new_internal_actions
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_internal_actions
    }

    template<action_signature Sig, class EventFilter, class Action>
    [[nodiscard]] constexpr auto exit_action(const EventFilter& event_filter, const Action& action) const
    {
        const auto new_exit_actions = tuple_append
        (
            options_.exit_actions,
            detail::make_event_action<Sig>(event_filter, action)
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

#undef MAKI_DETAIL_STATE_CONF_RETURN_TYPE

} //namespace

#endif
