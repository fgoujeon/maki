//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::state_mold struct template
*/

#ifndef MAKI_STATE_BUILDER_HPP
#define MAKI_STATE_BUILDER_HPP

#include "action.hpp"
#include "context.hpp"
#include "event_set.hpp"
#include "detail/state_mold_impl.hpp"
#include "detail/type.hpp"
#include "detail/event_action.hpp"
#include "detail/signature_macros.hpp"
#include "detail/tuple.hpp"
#include "detail/friendly_impl.hpp"
#include "detail/tlu.hpp"
#include <string_view>
#include <type_traits>

namespace maki
{

#ifdef MAKI_DETAIL_DOXYGEN
#define MAKI_DETAIL_STATE_CONF_RETURN_TYPE state_mold<IMPLEMENTATION_DETAIL>
#else
#define MAKI_DETAIL_STATE_CONF_RETURN_TYPE auto
#endif

/**
@brief State mold
*/
template<class Impl>
class state_mold
{
public:
    using context_type = typename Impl::context_type;

    constexpr state_mold() = default;

    state_mold(const state_mold&) = delete;

    state_mold(state_mold&&) = delete;

    ~state_mold() = default;

    state_mold& operator=(const state_mold&) = delete;

    state_mold& operator=(state_mold&&) = delete;

#define MAKI_DETAIL_MAKE_STATE_CONF_COPY_BEGIN /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_context_type = detail::type<typename Impl::context_type>; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_context_sig = impl_.context_sig; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_entry_actions = impl_.entry_actions; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_internal_actions = impl_.internal_actions; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_exit_actions = impl_.exit_actions; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_pretty_name_view = impl_.pretty_name; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_transition_tables = impl_.transition_tables;

#define MAKI_DETAIL_MAKE_STATE_CONF_COPY_END /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    return state_mold \
    < \
        detail::state_mold_impl \
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
    @brief Sets the type of the context (see @ref maki::state_context_signature "signatures"). \
    */ \
    template<class Context> \
    [[nodiscard]] constexpr MAKI_DETAIL_STATE_CONF_RETURN_TYPE context_##signature() const \
    { \
        return context<Context, state_context_signature::signature>(); \
    }
    MAKI_DETAIL_STATE_CONTEXT_CONSTRUCTOR_SIGNATURES
#undef MAKI_DETAIL_X

#define MAKI_DETAIL_X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    /** \
    @brief Adds an entry action (see @ref maki::action_signature "signatures") \
    to be called for any event type in `event_types`. \
    */ \
    template<class EventSetImpl, class Action> \
    [[nodiscard]] constexpr MAKI_DETAIL_STATE_CONF_RETURN_TYPE entry_action_##signature(const event_set<EventSetImpl>& event_types, const Action& action) const \
    { \
        return entry_action<action_signature::signature>(event_types, action); \
    } \
 \
    /** \
    @brief Adds an entry action (see @ref maki::action_signature "signatures") \
    to be called for the event type `Event`. \
    */ \
    template<class Event, class Action> \
    [[nodiscard]] constexpr MAKI_DETAIL_STATE_CONF_RETURN_TYPE entry_action_##signature(const Action& action) const \
    { \
        return entry_action_##signature(event_set{event<Event>}, action); \
    } \
 \
    /** \
    @brief Adds an entry action (see @ref maki::action_signature "signatures") \
    to be called whatever the event type. \
    */ \
    template<class Action> \
    [[nodiscard]] constexpr MAKI_DETAIL_STATE_CONF_RETURN_TYPE entry_action_##signature(const Action& action) const \
    { \
        return entry_action_##signature(all_events, action); \
    }
    MAKI_DETAIL_ACTION_SIGNATURES
#undef MAKI_DETAIL_X

#define MAKI_DETAIL_X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    MAKI_DETAIL_ACTION_SIGNATURES
#undef MAKI_DETAIL_X

#define MAKI_DETAIL_X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    /** \
    @brief Adds an internal action (see @ref maki::action_signature "signatures") \
    to be called for any event type in `event_types`. \
    */ \
    template<class EventSetImpl, class Action> \
    [[nodiscard]] constexpr MAKI_DETAIL_STATE_CONF_RETURN_TYPE internal_action_##signature(const event_set<EventSetImpl>& event_types, const Action& action) const \
    { \
        return internal_action<action_signature::signature>(event_types, action); \
    } \
 \
    /** \
    @brief Adds an internal action (see @ref maki::action_signature "signatures") \
    to be called for the event type `Event`. \
    */ \
    template<class Event, class Action> \
    [[nodiscard]] constexpr MAKI_DETAIL_STATE_CONF_RETURN_TYPE internal_action_##signature(const Action& action) const \
    { \
        return internal_action_##signature(event_set{event<Event>}, action); \
    }
    MAKI_DETAIL_ACTION_SIGNATURES
#undef MAKI_DETAIL_X

#define MAKI_DETAIL_X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    /** \
    @brief Adds an exit action (see @ref maki::action_signature "signatures") \
    to be called for any event type in `event_types`. \
    */ \
    template<class EventSetImpl, class Action> \
    [[nodiscard]] constexpr MAKI_DETAIL_STATE_CONF_RETURN_TYPE exit_action_##signature(const event_set<EventSetImpl>& event_types, const Action& action) const \
    { \
        return exit_action<action_signature::signature>(event_types, action); \
    } \
 \
    /** \
    @brief Adds an exit action (see @ref maki::action_signature "signatures") \
    to be called for the event type `Event`. \
    */ \
    template<class Event, class Action> \
    [[nodiscard]] constexpr MAKI_DETAIL_STATE_CONF_RETURN_TYPE exit_action_##signature(const Action& action) const \
    { \
        return exit_action_##signature(event_set{event<Event>}, action); \
    } \
 \
    /** \
    @brief Adds an exit action (see @ref maki::action_signature "signatures") \
    to be called whatever the event type. \
    */ \
    template<class Action> \
    [[nodiscard]] constexpr MAKI_DETAIL_STATE_CONF_RETURN_TYPE exit_action_##signature(const Action& action) const \
    { \
        return exit_action_##signature(all_events, action); \
    }
    MAKI_DETAIL_ACTION_SIGNATURES
#undef MAKI_DETAIL_X

    /**
    @brief Forces the value returned by `maki::state::pretty_name()`.
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
        const auto tpl = detail::tuple<TransitionTables...>{tables...};
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_transition_tables tpl
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_transition_tables
    }

private:
    MAKI_DETAIL_FRIENDLY_IMPL

    using impl_type = Impl;

    template<class Impl2>
    friend class state_mold;

    template<class... Args>
    constexpr state_mold(Args&&... args):
        impl_{std::forward<Args>(args)...}
    {
    }

    template<class Context, auto ContextSig>
    [[nodiscard]] constexpr auto context() const
    {
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_context_type detail::type<Context>
#define MAKI_DETAIL_ARG_context_sig ContextSig
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_context_type
#undef MAKI_DETAIL_ARG_context_sig
    }

    template<action_signature Sig, class EventSetImpl, class Action>
    [[nodiscard]] constexpr auto entry_action(const event_set<EventSetImpl>& /*event_types*/, const Action& action) const
    {
        const auto new_entry_actions = append
        (
            impl_.entry_actions,
            detail::make_event_action<Sig, EventSetImpl>(action)
        );

        MAKI_DETAIL_MAKE_STATE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_entry_actions new_entry_actions
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_entry_actions
    }

    template<action_signature Sig, class EventSetImpl, class Action>
    [[nodiscard]] constexpr auto internal_action(const event_set<EventSetImpl>& /*event_types*/, const Action& action) const
    {
        const auto new_internal_actions = append
        (
            impl_.internal_actions,
            detail::make_event_action<Sig, EventSetImpl>(action)
        );

        MAKI_DETAIL_MAKE_STATE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_internal_actions new_internal_actions
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_internal_actions
    }

    template<action_signature Sig, class EventSetImpl, class Action>
    [[nodiscard]] constexpr auto exit_action(const event_set<EventSetImpl>& /*event_types*/, const Action& action) const
    {
        const auto new_exit_actions = append
        (
            impl_.exit_actions,
            detail::make_event_action<Sig, EventSetImpl>(action)
        );

        MAKI_DETAIL_MAKE_STATE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_exit_actions new_exit_actions
        MAKI_DETAIL_MAKE_STATE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_exit_actions
    }

#undef MAKI_DETAIL_MAKE_STATE_CONF_COPY_END
#undef MAKI_DETAIL_MAKE_STATE_CONF_COPY_BEGIN

    impl_type impl_;
};

#undef MAKI_DETAIL_STATE_CONF_RETURN_TYPE

#ifdef MAKI_DETAIL_DOXYGEN
state_mold() -> state_mold<IMPLEMENTATION_DETAIL>;
#else
state_mold() -> state_mold<detail::state_mold_impl<>>;
#endif

namespace detail
{
    template<class T>
    struct is_state_mold
    {
        static constexpr auto value = false;
    };

    template<class OptionSet>
    struct is_state_mold<state_mold<OptionSet>>
    {
        static constexpr auto value = true;
    };

    template<class T>
    constexpr bool is_state_mold_v = is_state_mold<T>::value;
}

} //namespace

#endif
