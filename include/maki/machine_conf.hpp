//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::machine_conf struct template
*/

#ifndef MAKI_MACHINE_CONF_HPP
#define MAKI_MACHINE_CONF_HPP

#include "transition_table.hpp"
#include "type_patterns.hpp"
#include "type_list.hpp"
#include "type.hpp"
#include "detail/noop_ex.hpp"
#include "detail/event_action.hpp"
#include "detail/tuple.hpp"
#include "detail/tlu.hpp"
#include <string_view>

namespace maki
{

/**
@brief @ref machine configuration
*/
template
<
    class ContextTypeHolder = type<void>,
    class EntryActionTuple = detail::tuple<>,
    class EventActionTuple = detail::tuple<>,
    class ExitActionTuple = detail::tuple<>,
    class ExceptionAction = detail::noop_ex,
    class PreStateTransitionAction = detail::noop_ex,
    class PostStateTransitionAction = detail::noop_ex,
    class FallbackTransitionActionTuple = detail::tuple<>,
    class TransitionTableTypeList = type_list<>
>
struct machine_conf
{
    using data_type = void; //TODO remove this
    using context_type = typename ContextTypeHolder::type;

    using exception_action_type = ExceptionAction;
    using pre_state_transition_action_type = PreStateTransitionAction;
    using post_state_transition_action_type = PostStateTransitionAction;
    using fallback_transition_action_tuple_type = FallbackTransitionActionTuple;

    /**
    @brief Specifies whether the constructor of @ref machine must call @ref machine::start().
    */
    bool auto_start = true; //NOLINT(misc-non-private-member-variables-in-classes)

    /**
    @brief Specifies the context type.
    */
    ContextTypeHolder context; //NOLINT(misc-non-private-member-variables-in-classes)

    EntryActionTuple entry_actions; //NOLINT(misc-non-private-member-variables-in-classes)

    EventActionTuple internal_actions; //NOLINT(misc-non-private-member-variables-in-classes)

    ExitActionTuple exit_actions; //NOLINT(misc-non-private-member-variables-in-classes)

    /**
    @brief Specifies whether @ref machine must call a user-provided
    `after_state_transition()` member function after any external state
    transition.

    The following expression must be valid, for every possible template argument
    list:
    @code
    machine_def.after_state_transition
    <
        region_path,
        source_state_type,
        event_type,
        target_state_type
    >(event);
    @endcode

    This hook can be useful for logging state transitions, for example.

    Example:
    @code
    struct machine_def
    {
        using conf = maki::machine_conf
            .enable_after_state_transition()
            //...
        ;

        template<const auto& RegionPath, class SourceState, class Event, class TargetState>
        void after_state_transition(const Event& event)
        {
            //...
        }

        //...
    };
    @endcode
    */
    PostStateTransitionAction post_state_transition_action_; //NOLINT(misc-non-private-member-variables-in-classes)

    /**
    @brief Specifies whether @ref machine must call a user-provided
    `before_state_transition()` member function before any external state
    transition.

    The following expression must be valid, for every possible template argument
    list:
    @code
    machine_def.before_state_transition
    <
        region_path,
        source_state_type,
        event_type,
        target_state_type
    >(event);
    @endcode

    This hook can be useful for logging state transitions, for example.

    Example:
    @code
    struct machine_def
    {
        using conf = maki::machine_conf
            .enable_before_state_transition()
            //...
        ;

        template<const auto& RegionPath, class SourceState, class Event, class TargetState>
        void before_state_transition(const Event& event)
        {
            //...
        }

        //...
    };
    @endcode
    */
    PreStateTransitionAction pre_state_transition_action_; //NOLINT(misc-non-private-member-variables-in-classes)

    /**
    @brief Specifies whether @ref machine must call a user-provided
    `on_exception()` member function whenever it catches an exception.

    The following expression must be valid:
    @code
    machine_def.on_exception(std::current_exception());
    @endcode

    If this option isn't set, @ref machine will send itself an @ref
    events::exception event, like so:
    @code
    process_event(events::exception{std::current_exception()});
    @endcode

    Example:
    @code
    struct machine_def
    {
        static constexpr auto conf = default_machine_conf
            .enable_on_exception()
            //...
        ;

        void on_exception(const std::exception_ptr& eptr)
        {
            //...
        }

        //...
    };
    @endcode
    */
    ExceptionAction exception_action_; //NOLINT(misc-non-private-member-variables-in-classes)

    /**
    @brief Specifies whether @ref machine must call a user-provided
    `on_unprocessed()` member function whenever a call to @ref
    machine::process_event() doesn't lead to any state transition or call to any
    `on_event()` function.

    The said member function must have the following form:
    @code
    void on_unprocessed(const event_type& event);
    @endcode

    State machine definitions typically define several overloads of this
    function, for all events of interest.

    Example:
    @code
    struct machine_def
    {
        static constexpr auto conf = default_machine_conf
            .enable_on_unprocessed()
            //...
        ;

        void on_unprocessed(const some_event_type& event)
        {
            //...
        }

        void on_unprocessed(const some_other_event_type& event)
        {
            //...
        }

        //Ignore all other event types
        template<class Event>
        void on_unprocessed(const Event&)
        {
            //nothing
        }

        //...
    };
    @endcode
    */
    FallbackTransitionActionTuple fallback_transition_actions_; //NOLINT(misc-non-private-member-variables-in-classes)

    /**
    @brief Specifies whether @ref machine must call a user-provided
    `pretty_name()` static member function to get the pretty name of the state
    machine.

    See `maki::pretty_name`.

    Example:
    @code
    struct machine_def
    {
        static constexpr auto conf = default_machine_conf
            .enable_pretty_name()
            //...
        ;

        static auto pretty_name()
        {
            return "Main FSM";
        }

        //...
    };
    @endcode
    */
    std::string_view pretty_name_view; //NOLINT(misc-non-private-member-variables-in-classes)

    /**
    @brief Specifies whether run-to-completion is enabled.

    Disabling run-to-completion makes the state machine much faster, but you
    have to make sure you <b>never</b> call @ref machine::process_event()
    recursively.

    Disable it at your own risk!
    */
    bool run_to_completion = true; //NOLINT(misc-non-private-member-variables-in-classes)

    /**
    @brief Maximum object alignment requirement for the run-to-completion event
    queue to enable small object optimization (and thus avoid an extra memory
    allocation).
    */
    std::size_t small_event_max_align = 8; //NOLINT(misc-non-private-member-variables-in-classes, cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

    /**
    @brief Maximum object size for the run-to-completion event queue to enable
    small object optimization (and thus avoid an extra memory allocation).
    */
    std::size_t small_event_max_size = 16; //NOLINT(misc-non-private-member-variables-in-classes, cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

    /**
    @brief The list of transition table types. One region per transmission table
    is created.
    */
    TransitionTableTypeList transition_tables; //NOLINT(misc-non-private-member-variables-in-classes)

#define MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_auto_start = auto_start; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_context = context; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_entry_actions = entry_actions; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_internal_actions = internal_actions; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_exit_actions = exit_actions; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_post_state_transition_action = post_state_transition_action_; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_pre_state_transition_action = pre_state_transition_action_; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_exception_action = exception_action_; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_fallback_transition_actions = fallback_transition_actions_; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_pretty_name_view = pretty_name_view; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_run_to_completion = run_to_completion; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_small_event_max_align = small_event_max_align; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_small_event_max_size = small_event_max_size; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_transition_tables = transition_tables;

#define MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    return machine_conf \
    < \
        std::decay_t<decltype(MAKI_DETAIL_ARG_context)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_entry_actions)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_internal_actions)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_exit_actions)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_exception_action)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_pre_state_transition_action)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_post_state_transition_action)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_fallback_transition_actions)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_transition_tables)> \
    > \
    { \
        MAKI_DETAIL_ARG_auto_start, \
        MAKI_DETAIL_ARG_context, \
        MAKI_DETAIL_ARG_entry_actions, \
        MAKI_DETAIL_ARG_internal_actions, \
        MAKI_DETAIL_ARG_exit_actions, \
        MAKI_DETAIL_ARG_post_state_transition_action, \
        MAKI_DETAIL_ARG_pre_state_transition_action, \
        MAKI_DETAIL_ARG_exception_action, \
        MAKI_DETAIL_ARG_fallback_transition_actions, \
        MAKI_DETAIL_ARG_pretty_name_view, \
        MAKI_DETAIL_ARG_run_to_completion, \
        MAKI_DETAIL_ARG_small_event_max_align, \
        MAKI_DETAIL_ARG_small_event_max_size, \
        MAKI_DETAIL_ARG_transition_tables \
    };

    template<class EventFilter, detail::event_action_signature Sig, class Action>
    [[nodiscard]] constexpr auto entry_action(const Action& action) const
    {
        const auto new_entry_actions = append
        (
            entry_actions,
            detail::event_action<EventFilter, Action, Sig>{action}
        );

        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_entry_actions new_entry_actions
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_entry_actions
    }

#define X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    template<class EventFilter, class Action> \
    [[nodiscard]] constexpr auto entry_action_##signature(const Action& action) const \
    { \
        return entry_action<EventFilter, detail::event_action_signature::signature>(action); \
    }
    MAKI_DETAIL_EVENT_ACTION_SIGNATURES
#undef X

    template<class EventFilter, detail::event_action_signature Sig, class Action>
    [[nodiscard]] constexpr auto event_action(const Action& action) const
    {
        const auto new_internal_actions = tuple_append
        (
            internal_actions,
            detail::event_action<EventFilter, Action, Sig>{action}
        );

        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_internal_actions new_internal_actions
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_internal_actions
    }

#define X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    template<class EventFilter, class Action> \
    [[nodiscard]] constexpr auto event_action_##signature(const Action& action) const \
    { \
        return event_action<EventFilter, detail::event_action_signature::signature>(action); \
    }
    MAKI_DETAIL_EVENT_ACTION_SIGNATURES
#undef X

    template<class EventFilter, detail::event_action_signature Sig, class Action>
    [[nodiscard]] constexpr auto exit_action(const Action& action) const
    {
        const auto new_exit_actions = append
        (
            exit_actions,
            detail::event_action<EventFilter, Action, Sig>{action}
        );

        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_exit_actions new_exit_actions
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_exit_actions
    }

#define X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    template<class EventFilter, class Action> \
    [[nodiscard]] constexpr auto exit_action_##signature(const Action& action) const \
    { \
        return exit_action<EventFilter, detail::event_action_signature::signature>(action); \
    }
    MAKI_DETAIL_EVENT_ACTION_SIGNATURES
#undef X

    template<class Action>
    [[nodiscard]] constexpr auto post_state_transition_action_crset(const Action& action) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_post_state_transition_action action
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_post_state_transition_action
    }

    [[nodiscard]] constexpr auto disable_auto_start() const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_auto_start false
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_auto_start
    }

    template<class Action>
    [[nodiscard]] constexpr auto pre_state_transition_action_crset(const Action& action) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_pre_state_transition_action action
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_pre_state_transition_action
    }

    template<class Context>
    [[nodiscard]] constexpr auto set_context() const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_context type_c<Context>
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_context
    }

    [[nodiscard]] constexpr auto disable_run_to_completion() const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_run_to_completion false
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_run_to_completion
    }

    [[nodiscard]] constexpr auto pretty_name(const std::string_view value) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_pretty_name_view value
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_pretty_name_view
    }

    template<class Action>
    [[nodiscard]] constexpr auto exception_action_me(const Action& action) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_exception_action action
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_exception_action
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto fallback_transition_action_me(const Action& action) const
    {
        const auto new_fallback_transition_actions = tuple_append
        (
            fallback_transition_actions_,
            detail::event_action<EventFilter, Action, detail::event_action_signature::me>{action}
        );

        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_fallback_transition_actions new_fallback_transition_actions
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_fallback_transition_actions
    }

    [[nodiscard]] constexpr auto set_small_event_max_align(const std::size_t value) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_small_event_max_align value
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_small_event_max_align
    }

    [[nodiscard]] constexpr auto set_small_event_max_size(const std::size_t value) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_small_event_max_size value
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_small_event_max_size
    }

    template<class... TransitionTables>
    [[nodiscard]] constexpr auto set_transition_tables(const TransitionTables&... /*tables*/) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_transition_tables type_list_c<TransitionTables...>
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_transition_tables
    }

#undef MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
};

inline constexpr auto default_machine_conf = machine_conf<>{};

namespace detail
{
    template<class T>
    struct is_root_sm_conf
    {
        static constexpr auto value = false;
    };

    template<class... Options>
    struct is_root_sm_conf<machine_conf<Options...>>
    {
        static constexpr auto value = true;
    };

    template<class T>
    constexpr auto is_root_sm_conf_v = is_root_sm_conf<T>::value;
}

} //namespace

#endif
