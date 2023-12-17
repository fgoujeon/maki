//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::machine_conf_t struct template
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

#ifdef DOXYGEN
/**
@brief @ref machine configuration
*/
template<IMPLEMENTATION_DETAIL>
#else
template
<
    class Context = void,
    class Data = void,
    class EntryActionTuple = detail::tuple<>,
    class EventActionTuple = detail::tuple<>,
    class ExitActionTuple = detail::tuple<>,
    class ExceptionAction = detail::noop_ex,
    class PreStateTransitionAction = detail::noop_ex,
    class PostStateTransitionAction = detail::noop_ex,
    class FallbackTransitionActionTuple = detail::tuple<>,
    class TransitionTableTypeList = type_list<>
>
#endif
class machine_conf_t
{
public:
    using data_type = Data;
    using context_type = Context;
    using exception_action_type = ExceptionAction;
    using pre_state_transition_action_type = PreStateTransitionAction;
    using post_state_transition_action_type = PostStateTransitionAction;
    using fallback_transition_action_tuple_type = FallbackTransitionActionTuple;

#define MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_auto_start = auto_start_; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_context_type = type_c<context_type>; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_data_type = type_c<data_type>; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_entry_actions = entry_actions_; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_internal_actions = internal_actions_; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_exit_actions = exit_actions_; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_post_state_transition_action = post_state_transition_action_; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_pre_state_transition_action = pre_state_transition_action_; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_exception_action = exception_action_; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_fallback_transition_actions = fallback_transition_actions_; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_pretty_name_view = pretty_name_; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_run_to_completion = run_to_completion_; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_small_event_max_align = small_event_max_align_; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_small_event_max_size = small_event_max_size_; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_transition_tables = transition_tables_;

#define MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    return machine_conf_t \
    < \
        typename std::decay_t<decltype(MAKI_DETAIL_ARG_context_type)>::type, \
        typename std::decay_t<decltype(MAKI_DETAIL_ARG_data_type)>::type, \
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

    /**
    @brief Specifies the data type.
    */
    template<class Data2>
    [[nodiscard]] constexpr auto data() const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_data_type type_c<Data2>
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_data_type
    }

#define X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    /** \
    @brief Adds an entry action. \
    */ \
    template<class EventFilter, class Action> \
    [[nodiscard]] constexpr auto entry_action_##signature(const Action& action) const \
    { \
        return entry_action<EventFilter, detail::event_action_signature::signature>(action); \
    }
    MAKI_DETAIL_EVENT_ACTION_SIGNATURES
#undef X

#define X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    /** \
    @brief Adds an action to be called whenever `maki::machine` is about to \
    process an event. \
    */ \
    template<class EventFilter, class Action> \
    [[nodiscard]] constexpr auto event_action_##signature(const Action& action) const \
    { \
        return event_action<EventFilter, detail::event_action_signature::signature>(action); \
    }
    MAKI_DETAIL_EVENT_ACTION_SIGNATURES
#undef X

#define X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    /** \
    @brief Adds an exit action. \
    */ \
    template<class EventFilter, class Action> \
    [[nodiscard]] constexpr auto exit_action_##signature(const Action& action) const \
    { \
        return exit_action<EventFilter, detail::event_action_signature::signature>(action); \
    }
    MAKI_DETAIL_EVENT_ACTION_SIGNATURES
#undef X

    /**
    @brief Specifies an action to be called after any external state transition.

    Action must have the following form:

    @code
    template
    <
        const auto& RegionPath,
        const auto& SourceStateConf,
        class Event,
        const auto& TargetStateConf
    >
    void action
    (
        context& ctx,
        const maki::cref_constant<RegionPath> region_path_constant,
        const maki::cref_constant<SourceStateConf> source_state_conf_constant,
        const Event& event,
        const maki::cref_constant<TargetStateConf> target_state_conf_constant
    );
    @endcode

    This hook can be useful for logging state transitions, for example:

    @code
    .pre_state_transition_action_crset
    (
        []
        (
            context& ctx,
            const auto region_path_constant,
            const auto source_state_constant,
            const auto& event,
            const auto target_state_constant
        )
        {
            std::cout
                << "Beginning of transition in "
                << region_path_constant.value.to_string()
                << ": "
                << maki::pretty_name<source_state_constant.value>()
                << " -> "
                << maki::pretty_name<target_state_constant.value>();
        }
    )
    @endcode
    */
    template<class Action>
    [[nodiscard]] constexpr auto post_state_transition_action_crset(const Action& action) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_post_state_transition_action action
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_post_state_transition_action
    }

    /**
    @brief Specifies whether the constructor of @ref machine must call @ref machine::start().
    */
    [[nodiscard]] constexpr auto auto_start(const bool value) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_auto_start value
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_auto_start
    }

    /**
    @brief Specifies an action to be called before any external state
    transition.

    Action must have the following form:

    @code
    template
    <
        const auto& RegionPath,
        const auto& SourceStateConf,
        class Event,
        const auto& TargetStateConf
    >
    void action
    (
        context& ctx,
        const maki::cref_constant<RegionPath> region_path_constant,
        const maki::cref_constant<SourceStateConf> source_state_conf_constant,
        const Event& event,
        const maki::cref_constant<TargetStateConf> target_state_conf_constant
    );
    @endcode

    This hook can be useful for logging state transitions, for example:

    @code
    .post_state_transition_action_crset
    (
        []
        (
            context& ctx,
            const auto region_path_constant,
            const auto source_state_constant,
            const auto& event,
            const auto target_state_constant
        )
        {
            std::cout
                << "End of transition in "
                << region_path_constant.value.to_string()
                << ": "
                << maki::pretty_name<source_state_constant.value>()
                << " -> "
                << maki::pretty_name<target_state_constant.value>();
        }
    )
    @endcode
    */
    template<class Action>
    [[nodiscard]] constexpr auto pre_state_transition_action_crset(const Action& action) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_pre_state_transition_action action
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_pre_state_transition_action
    }

    /**
    @brief Specifies the context type.
    */
    template<class Context2>
    [[nodiscard]] constexpr auto context() const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_context_type type_c<Context2>
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_context_type
    }

    /**
    @brief Specifies whether run-to-completion is enabled.

    Disabling run-to-completion makes the state machine much faster, but you
    have to make sure you <b>never</b> call `machine::process_event()`
    recursively.

    Disable it at your own risk!
    */
    [[nodiscard]] constexpr auto run_to_completion(const bool value) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_run_to_completion value
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_run_to_completion
    }

    /**
    @brief Specifies the pretty name of the state machine.

    See `maki::pretty_name`.
    */
    [[nodiscard]] constexpr auto pretty_name(const std::string_view value) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_pretty_name_view value
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_pretty_name_view
    }

    /**
    @brief Specifies an action to be called whenever an exception is caught by
    `machine`.

    As the suffix of the function suggests, the action must take references to:

    - the machine;
    - the event.

    Example:
    @code
    .exception_action_me([](auto& mach, const auto& event)
    {
        //...
    })
    @endcode

    If this action isn't set, `maki::machine` will send itself a
    `maki::events::exception` event, like so:
    @code
    process_event(maki::events::exception{std::current_exception()});
    @endcode
    */
    template<class Action>
    [[nodiscard]] constexpr auto exception_action_me(const Action& action) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_exception_action action
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_exception_action
    }

    /**
    @brief Adds an action to be invoked whenever a call to
    `maki::machine::process_event()` doesn't lead to any state transition or
    call to any kind of action.

    Users typically add an action for every event of interest.

    Example:
    @code
    constexpr auto conf = maki::machine_conf
        //...
        .fallback_transition_action_me<some_event_type>([](auto& mach, const some_event_type& event)
        {
            //...
        })
        .fallback_transition_action_me<some_other_event_type>([](auto& mach, const some_other_event_type& event)
        {
            //...
        })
    ;
    @endcode
    */
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

    /**
    @brief Specifies the maximum object alignment requirement for the
    run-to-completion event queue to enable small object optimization (and thus
    avoid an extra memory allocation).
    */
    [[nodiscard]] constexpr auto small_event_max_align(const std::size_t value) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_small_event_max_align value
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_small_event_max_align
    }

    /**
    @brief Specifies the maximum object size for the run-to-completion event
    queue to enable small object optimization (and thus avoid an extra memory
    allocation).
    */
    [[nodiscard]] constexpr auto small_event_max_size(const std::size_t value) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_small_event_max_size value
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_small_event_max_size
    }

    /**
    @brief Specifies the list of transition tables. One region per
    transmission table is created.
    */
    template<class... TransitionTables>
    [[nodiscard]] constexpr auto transition_tables(const TransitionTables&... /*tables*/) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_transition_tables type_list_c<TransitionTables...>
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_transition_tables
    }

#if DOXYGEN
private:
#endif
    template<class EventFilter, detail::event_action_signature Sig, class Action>
    [[nodiscard]] constexpr auto entry_action(const Action& action) const
    {
        const auto new_entry_actions = tuple_append
        (
            entry_actions_,
            detail::event_action<EventFilter, Action, Sig>{action}
        );

        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_entry_actions new_entry_actions
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_entry_actions
    }

    template<class EventFilter, detail::event_action_signature Sig, class Action>
    [[nodiscard]] constexpr auto event_action(const Action& action) const
    {
        const auto new_internal_actions = tuple_append
        (
            internal_actions_,
            detail::event_action<EventFilter, Action, Sig>{action}
        );

        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_internal_actions new_internal_actions
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_internal_actions
    }

    template<class EventFilter, detail::event_action_signature Sig, class Action>
    [[nodiscard]] constexpr auto exit_action(const Action& action) const
    {
        const auto new_exit_actions = tuple_append
        (
            exit_actions_,
            detail::event_action<EventFilter, Action, Sig>{action}
        );

        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_exit_actions new_exit_actions
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_exit_actions
    }

#undef MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN

    bool auto_start_ = true; //NOLINT(misc-non-private-member-variables-in-classes)
    EntryActionTuple entry_actions_; //NOLINT(misc-non-private-member-variables-in-classes)
    EventActionTuple internal_actions_; //NOLINT(misc-non-private-member-variables-in-classes)
    ExitActionTuple exit_actions_; //NOLINT(misc-non-private-member-variables-in-classes)
    PostStateTransitionAction post_state_transition_action_; //NOLINT(misc-non-private-member-variables-in-classes)
    PreStateTransitionAction pre_state_transition_action_; //NOLINT(misc-non-private-member-variables-in-classes)
    ExceptionAction exception_action_; //NOLINT(misc-non-private-member-variables-in-classes)
    FallbackTransitionActionTuple fallback_transition_actions_; //NOLINT(misc-non-private-member-variables-in-classes)
    std::string_view pretty_name_; //NOLINT(misc-non-private-member-variables-in-classes)
    bool run_to_completion_ = true; //NOLINT(misc-non-private-member-variables-in-classes)
    std::size_t small_event_max_align_ = 8; //NOLINT(misc-non-private-member-variables-in-classes, cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    std::size_t small_event_max_size_ = 16; //NOLINT(misc-non-private-member-variables-in-classes, cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    TransitionTableTypeList transition_tables_; //NOLINT(misc-non-private-member-variables-in-classes)
};

inline constexpr auto machine_conf = machine_conf_t<>{};

namespace detail
{
    template<class T>
    struct is_root_sm_conf
    {
        static constexpr auto value = false;
    };

    template<class... Options>
    struct is_root_sm_conf<machine_conf_t<Options...>>
    {
        static constexpr auto value = true;
    };

    template<class T>
    constexpr auto is_root_sm_conf_v = is_root_sm_conf<T>::value;
}

} //namespace

#endif
