//Copyright Florian Goujeon 2021 - 2024.
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

#include "machine_conf_fwd.hpp"
#include "type.hpp"
#include "detail/context_signature.hpp"
#include "detail/event_action.hpp"
#include "detail/tuple.hpp"
#include <string_view>
#include <type_traits>
#include <cstdlib>

namespace maki
{

namespace detail
{
    //Read access to options
    template<class OptionSet>
    constexpr const auto& opts(const machine_conf<OptionSet>& conf)
    {
        return conf.options_;
    }
}

#ifdef MAKI_DETAIL_DOXYGEN
template<IMPLEMENTATION_DETAIL>
#else
template<class OptionSet>
#endif
class machine_conf
{
public:
    using context_type = typename OptionSet::context_type;

    constexpr machine_conf() = default;

    machine_conf(const machine_conf&) = delete;

    machine_conf(machine_conf&&) = delete;

    ~machine_conf() = default;

    machine_conf& operator=(const machine_conf&) = delete;

    machine_conf& operator=(machine_conf&&) = delete;

#define MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_auto_start = options_.auto_start; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_context_type = type<typename OptionSet::context_type>; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_context_sig = options_.context_sig; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_entry_actions = options_.entry_actions; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_internal_actions = options_.internal_actions; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_exit_actions = options_.exit_actions; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_post_state_transition_hook = options_.post_state_transition_hook; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_pre_state_transition_hook = options_.pre_state_transition_hook; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_exception_hook = options_.exception_hook; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_post_processing_hooks = options_.post_processing_hooks; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_pretty_name_view = options_.pretty_name; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_run_to_completion = options_.run_to_completion; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_small_event_max_align = options_.small_event_max_align; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_small_event_max_size = options_.small_event_max_size; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_transition_tables = options_.transition_tables;

#define MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    return machine_conf \
    < \
        detail::machine_conf_option_set \
        < \
            typename std::decay_t<decltype(MAKI_DETAIL_ARG_context_type)>::type, \
            std::decay_t<decltype(MAKI_DETAIL_ARG_entry_actions)>, \
            std::decay_t<decltype(MAKI_DETAIL_ARG_internal_actions)>, \
            std::decay_t<decltype(MAKI_DETAIL_ARG_exit_actions)>, \
            std::decay_t<decltype(MAKI_DETAIL_ARG_exception_hook)>, \
            std::decay_t<decltype(MAKI_DETAIL_ARG_pre_state_transition_hook)>, \
            std::decay_t<decltype(MAKI_DETAIL_ARG_post_state_transition_hook)>, \
            std::decay_t<decltype(MAKI_DETAIL_ARG_post_processing_hooks)>, \
            std::decay_t<decltype(MAKI_DETAIL_ARG_transition_tables)> \
        > \
    > \
    { \
        MAKI_DETAIL_ARG_auto_start, \
        MAKI_DETAIL_ARG_context_sig, \
        MAKI_DETAIL_ARG_entry_actions, \
        MAKI_DETAIL_ARG_internal_actions, \
        MAKI_DETAIL_ARG_exit_actions, \
        MAKI_DETAIL_ARG_post_state_transition_hook, \
        MAKI_DETAIL_ARG_pre_state_transition_hook, \
        MAKI_DETAIL_ARG_exception_hook, \
        MAKI_DETAIL_ARG_post_processing_hooks, \
        MAKI_DETAIL_ARG_pretty_name_view, \
        MAKI_DETAIL_ARG_run_to_completion, \
        MAKI_DETAIL_ARG_small_event_max_align, \
        MAKI_DETAIL_ARG_small_event_max_size, \
        MAKI_DETAIL_ARG_transition_tables \
    };

#define MAKI_DETAIL_X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    template<class Context> \
    [[nodiscard]] constexpr auto context_##signature(const type_t<Context>& /*ignored*/) const \
    { \
        return context<Context, detail::context_signature::signature>(); \
    }
    MAKI_DETAIL_CONTEXT_SIGNATURES_FOR_MACHINE
#undef MAKI_DETAIL_X

#define MAKI_DETAIL_X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    /** \
    @brief Adds a start action. \
    */ \
    template<class EventFilter, class Action> \
    [[nodiscard]] constexpr auto start_action_##signature(const EventFilter& event_filter, const Action& action) const \
    { \
        return start_action<detail::event_action_signature::signature>(event_filter, action); \
    }
    MAKI_DETAIL_EVENT_ACTION_SIGNATURES
#undef MAKI_DETAIL_X

#define MAKI_DETAIL_X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    /** \
    @brief Adds a hook to be called whenever `maki::machine` is about to \
    process an event. \
    */ \
    template<class EventFilter, class Action> \
    [[nodiscard]] constexpr auto pre_processing_hook_##signature(const EventFilter& event_filter, const Action& action) const \
    { \
        return pre_processing_hook<detail::event_action_signature::signature>(event_filter, action); \
    }
    MAKI_DETAIL_EVENT_ACTION_SIGNATURES
#undef MAKI_DETAIL_X

#define MAKI_DETAIL_X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    /** \
    @brief Adds a stop action. \
    */ \
    template<class EventFilter, class Action> \
    [[nodiscard]] constexpr auto stop_action_##signature(const EventFilter& event_filter, const Action& action) const \
    { \
        return stop_action<detail::event_action_signature::signature>(event_filter, action); \
    }
    MAKI_DETAIL_EVENT_ACTION_SIGNATURES
#undef MAKI_DETAIL_X

    /**
    @brief Specifies a hook to be called after any external state transition.

    Hook must have the following form:

    @code
    template
    <
        const auto& RegionPath,
        const auto& SourceStateConf,
        class Event,
        const auto& TargetStateConf
    >
    void hook
    (
        context& ctx,
        const maki::cref_constant_t<RegionPath> path_constant,
        const maki::cref_constant_t<SourceStateConf> source_state_conf_constant,
        const Event& event,
        const maki::cref_constant_t<TargetStateConf> target_state_conf_constant
    );
    @endcode

    This hook can be useful for logging state transitions, for example:

    @code
    .post_state_transition_hook_crset
    (
        []
        (
            context& ctx,
            const auto path_constant,
            const auto source_state_constant,
            const auto& event,
            const auto target_state_constant
        )
        {
            std::cout
                << "Beginning of transition in "
                << path_constant.value.to_string()
                << ": "
                << maki::pretty_name<source_state_constant.value>()
                << " -> "
                << maki::pretty_name<target_state_constant.value>();
        }
    )
    @endcode
    */
    template<class Hook>
    [[nodiscard]] constexpr auto post_state_transition_hook_crset(const Hook& hook) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_post_state_transition_hook hook
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_post_state_transition_hook
    }

    /**
    @brief Specifies whether the constructor of `maki::machine` must call
    `maki::machine::start()`.
    */
    [[nodiscard]] constexpr auto auto_start(const bool value) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_auto_start value
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_auto_start
    }

    /**
    @brief Specifies a hook to be called before any external state
    transition.

    Hook must have the following form:

    @code
    template
    <
        const auto& RegionPath,
        const auto& SourceStateConf,
        class Event,
        const auto& TargetStateConf
    >
    void hook
    (
        context& ctx,
        const maki::cref_constant_t<RegionPath> path_constant,
        const maki::cref_constant_t<SourceStateConf> source_state_conf_constant,
        const Event& event,
        const maki::cref_constant_t<TargetStateConf> target_state_conf_constant
    );
    @endcode

    This hook can be useful for logging state transitions, for example:

    @code
    .pre_state_transition_hook_crset
    (
        []
        (
            context& ctx,
            const auto path_constant,
            const auto source_state_constant,
            const auto& event,
            const auto target_state_constant
        )
        {
            std::cout
                << "End of transition in "
                << path_constant.value.to_string()
                << ": "
                << maki::pretty_name<source_state_constant.value>()
                << " -> "
                << maki::pretty_name<target_state_constant.value>();
        }
    )
    @endcode
    */
    template<class Hook>
    [[nodiscard]] constexpr auto pre_state_transition_hook_crset(const Hook& hook) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_pre_state_transition_hook hook
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_pre_state_transition_hook
    }

    /**
    @brief Specifies whether run-to-completion is enabled.

    Disabling run-to-completion makes the state machine much faster, but you
    have to make sure you <b>never</b> call `maki::machine::process_event()`
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

    See `maki::pretty_name()`.
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
    - the exception pointer.

    Example:
    @code
    .exception_hook_mx([](auto& mach, const std::exception_ptr& eptr)
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
    [[nodiscard]] constexpr auto exception_hook_mx(const Action& action) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_exception_hook action
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_exception_hook
    }

    /**
    @brief Adds a hook to be called whenever `maki::machine` is done processing
    an event.

    Hook must have the following form:

    @code
    template<class Machine, class Event>
    void hook(Machine& mach, const Event& event, bool processed);
    @endcode

    The `processed` parameter indicates whether a state transition or any kind
    of action invocation (note: a hook isn't an action) occurred during the
    processing of the event.

    Users typically add a hook for every event of interest.

    Example:
    @code
    constexpr auto conf = maki::machine_conf{}
        //...
        .post_processing_hook_mep<some_event_type>([](auto& mach, const some_event_type& event, const bool processed)
        {
            //...
        })
        .post_processing_hook_mep<some_other_event_type>([](auto& mach, const some_other_event_type& event, const bool processed)
        {
            //...
        })
    ;
    @endcode
    */
    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto post_processing_hook_mep(const EventFilter& event_filter, const Action& action) const
    {
        const auto new_post_processing_hooks = tuple_append
        (
            options_.post_processing_hooks,
            detail::make_event_action<detail::event_action_signature::me>(event_filter, action)
        );

        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_post_processing_hooks new_post_processing_hooks
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_post_processing_hooks
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
    [[nodiscard]] constexpr auto transition_tables(const TransitionTables&... tables) const
    {
        const auto tpl = detail::tuple<TransitionTables...>{detail::distributed_construct, tables...};
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_transition_tables tpl
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_transition_tables
    }

private:
    template<class OptionSet2>
    friend class machine_conf;

    template<class OptionSet2>
    friend constexpr const auto& detail::opts(const machine_conf<OptionSet2>& conf);

    template<class... Args>
    constexpr machine_conf(Args&&... args):
        options_{std::forward<Args>(args)...}
    {
    }

    template<class Context2, auto ContextSig>
    [[nodiscard]] constexpr auto context() const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_context_type type<Context2>
#define MAKI_DETAIL_ARG_context_sig ContextSig
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_context_type
#undef MAKI_DETAIL_ARG_context_sig
    }

    template<detail::event_action_signature Sig, class EventFilter, class Action>
    [[nodiscard]] constexpr auto start_action(const EventFilter& event_filter, const Action& action) const
    {
        const auto new_entry_actions = tuple_append
        (
            options_.entry_actions,
            detail::make_event_action<Sig>(event_filter, action)
        );

        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_entry_actions new_entry_actions
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_entry_actions
    }

    template<detail::event_action_signature Sig, class EventFilter, class Hook>
    [[nodiscard]] constexpr auto pre_processing_hook(const EventFilter& event_filter, const Hook& hook) const
    {
        const auto new_internal_actions = tuple_append
        (
            options_.internal_actions,
            detail::make_event_action<Sig>(event_filter, hook)
        );

        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_internal_actions new_internal_actions
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_internal_actions
    }

    template<detail::event_action_signature Sig, class EventFilter, class Action>
    [[nodiscard]] constexpr auto stop_action(const EventFilter& event_filter, const Action& action) const
    {
        const auto new_exit_actions = tuple_append
        (
            options_.exit_actions,
            detail::make_event_action<Sig>(event_filter, action)
        );

        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_exit_actions new_exit_actions
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_exit_actions
    }

#undef MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN

    OptionSet options_;
};

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
