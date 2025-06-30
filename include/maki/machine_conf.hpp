//Copyright Florian Goujeon 2021 - 2025.
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

#include "event_set.hpp"
#include "context.hpp"
#include "action.hpp"
#include "detail/machine_conf_fwd.hpp"
#include "detail/type.hpp"
#include "detail/event_action.hpp"
#include "detail/signature_macros.hpp"
#include "detail/impl.hpp"
#include "detail/tuple.hpp"
#include <type_traits>
#include <cstdlib>

namespace maki
{

#ifdef MAKI_DETAIL_DOXYGEN
#define MAKI_DETAIL_MACHINE_CONF_RETURN_TYPE machine_conf<IMPLEMENTATION_DETAIL>
#else
#define MAKI_DETAIL_MACHINE_CONF_RETURN_TYPE auto
#endif

/**
@brief `maki::machine` configuration
@tparam Impl implementation detail
*/
template<class Impl>
class machine_conf
{
public:
    using context_type = typename Impl::context_type;

    constexpr machine_conf() = default;

    machine_conf(const machine_conf&) = delete;

    machine_conf(machine_conf&&) = delete;

    ~machine_conf() = default;

    machine_conf& operator=(const machine_conf&) = delete;

    machine_conf& operator=(machine_conf&&) = delete;

#define MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_auto_start = impl_.auto_start; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_context_type = detail::type<typename Impl::context_type>; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_context_sig = impl_.context_sig; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_pre_processing_hooks = impl_.pre_processing_hooks; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_post_external_transition_hook = impl_.post_external_transition_hook; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_pre_external_transition_hook = impl_.pre_external_transition_hook; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_exception_handler = impl_.exception_handler; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_post_processing_hooks = impl_.post_processing_hooks; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_process_event_now_enabled = impl_.process_event_now_enabled; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_run_to_completion = impl_.run_to_completion; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_small_event_max_align = impl_.small_event_max_align; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_small_event_max_size = impl_.small_event_max_size; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_transition_tables = impl_.transition_tables;

#define MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    return machine_conf \
    < \
        detail::machine_conf_option_set \
        < \
            typename std::decay_t<decltype(MAKI_DETAIL_ARG_context_type)>::type, \
            std::decay_t<decltype(MAKI_DETAIL_ARG_pre_processing_hooks)>, \
            std::decay_t<decltype(MAKI_DETAIL_ARG_exception_handler)>, \
            std::decay_t<decltype(MAKI_DETAIL_ARG_pre_external_transition_hook)>, \
            std::decay_t<decltype(MAKI_DETAIL_ARG_post_external_transition_hook)>, \
            std::decay_t<decltype(MAKI_DETAIL_ARG_post_processing_hooks)>, \
            std::decay_t<decltype(MAKI_DETAIL_ARG_transition_tables)> \
        > \
    > \
    { \
        MAKI_DETAIL_ARG_auto_start, \
        MAKI_DETAIL_ARG_context_sig, \
        MAKI_DETAIL_ARG_pre_processing_hooks, \
        MAKI_DETAIL_ARG_post_external_transition_hook, \
        MAKI_DETAIL_ARG_pre_external_transition_hook, \
        MAKI_DETAIL_ARG_exception_handler, \
        MAKI_DETAIL_ARG_post_processing_hooks, \
        MAKI_DETAIL_ARG_process_event_now_enabled, \
        MAKI_DETAIL_ARG_run_to_completion, \
        MAKI_DETAIL_ARG_small_event_max_align, \
        MAKI_DETAIL_ARG_small_event_max_size, \
        MAKI_DETAIL_ARG_transition_tables \
    };

#define MAKI_DETAIL_X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    /** \
    @brief Sets the type of the context (see @ref maki::machine_context_signature "signatures"). \
    */ \
    template<class Context> \
    [[nodiscard]] constexpr MAKI_DETAIL_MACHINE_CONF_RETURN_TYPE context_##signature() const \
    { \
        return context<Context, machine_context_signature::signature>(); \
    }
    MAKI_DETAIL_MACHINE_CONTEXT_CONSTRUCTOR_SIGNATURES
#undef MAKI_DETAIL_X

#define MAKI_DETAIL_X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    /** \
    @brief Adds a hook (see @ref maki::action_signature "signatures") to be \
    called whenever `maki::machine` is about to process an event whose type is \
    part of `evt_set`. \
    */ \
    template<class EventSetPredicate, class Action> \
    [[nodiscard]] constexpr MAKI_DETAIL_MACHINE_CONF_RETURN_TYPE pre_processing_hook_##signature(const event_set<EventSetPredicate>& evt_set, const Action& action) const \
    { \
        return pre_processing_hook<action_signature::signature>(evt_set, action); \
    } \
 \
    /** \
    @brief Adds a hook (see @ref maki::action_signature "signatures") to be \
    called whenever `maki::machine` is about to process an event of type \
    `Event`. \
    */ \
    template<class Event, class Action> \
    [[nodiscard]] constexpr MAKI_DETAIL_MACHINE_CONF_RETURN_TYPE pre_processing_hook_##signature(const Action& action) const \
    { \
        return pre_processing_hook_##signature(event_set{event<Event>}, action); \
    }
    MAKI_DETAIL_ACTION_SIGNATURES
#undef MAKI_DETAIL_X

    /**
    @brief Specifies a hook to be called after any external transition.

    Hook must have the following form:

    @snippet misc/external-transition-hook/src/main.cpp signature

    This hook can be useful for logging external transitions, for example:

    @snippet misc/external-transition-hook/src/main.cpp post
    */
    template<class Hook>
    [[nodiscard]] constexpr MAKI_DETAIL_MACHINE_CONF_RETURN_TYPE post_external_transition_hook_crste(const Hook& hook) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_post_external_transition_hook hook
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_post_external_transition_hook
    }

    /**
    @brief Specifies whether the constructor of `maki::machine` must call
    `maki::machine::start()`.
    */
    [[nodiscard]] constexpr MAKI_DETAIL_MACHINE_CONF_RETURN_TYPE auto_start(const bool value) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_auto_start value
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_auto_start
    }

    /**
    @brief Specifies a hook to be called before any external transition.

    Hook must have the following form:

    @snippet misc/external-transition-hook/src/main.cpp signature

    This hook can be useful for logging external transitions, for example:

    @snippet misc/external-transition-hook/src/main.cpp pre
    */
    template<class Hook>
    [[nodiscard]] constexpr MAKI_DETAIL_MACHINE_CONF_RETURN_TYPE pre_external_transition_hook_crste(const Hook& hook) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_pre_external_transition_hook hook
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_pre_external_transition_hook
    }

    /**
    @brief Specifies whether run-to-completion is enabled.

    Disabling run-to-completion makes the state machine much faster, but you
    have to make sure you <b>never</b> call `maki::machine::process_event()`
    recursively.

    Disable it at your own risk!
    */
    [[nodiscard]] constexpr MAKI_DETAIL_MACHINE_CONF_RETURN_TYPE run_to_completion(const bool value) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_run_to_completion value
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_run_to_completion
    }

    /**
    @brief Specifies whether the unsafe function
    `maki::machine::process_event_now()` can be called.
    */
    [[nodiscard]] constexpr MAKI_DETAIL_MACHINE_CONF_RETURN_TYPE process_event_now_enabled(const bool value) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_process_event_now_enabled value
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_process_event_now_enabled
    }

    /**
    @brief Sets up a `try`/`catch(...)` block at the top-level of every
    non-`const` member function of `maki::machine` and invokes `callable` from
    the `catch` block.

    As the suffix of the function suggests, `callable` must take references to:

    - the machine;
    - the exception pointer.

    Example:
    @code
    .catch_mx([](auto& mach, const std::exception_ptr& eptr)
    {
        //...
    })
    @endcode

    If this action isn't set, `maki::machine` doesn't catch any exception.
    */
    template<class Callable>
    [[nodiscard]] constexpr MAKI_DETAIL_MACHINE_CONF_RETURN_TYPE catch_mx(const Callable& callable) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_exception_handler callable
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_exception_handler
    }

    /**
    @brief Adds a hook to be called whenever `maki::machine` is done processing
    an event whose type is part of `evt_set`.

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
    template<class EventSetPredicate, class Action>
    [[nodiscard]] constexpr MAKI_DETAIL_MACHINE_CONF_RETURN_TYPE post_processing_hook_mep(const event_set<EventSetPredicate>& evt_set, const Action& action) const
    {
        const auto new_post_processing_hooks = tuple_append
        (
            impl_.post_processing_hooks,
            detail::make_event_action<action_signature::me>(evt_set, action)
        );

        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_post_processing_hooks new_post_processing_hooks
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_post_processing_hooks
    }

    /**
    @brief Adds a hook to be called whenever `maki::machine` is done processing
    an event of type `Event`.

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
    template<class Event, class Action>
    [[nodiscard]] constexpr MAKI_DETAIL_MACHINE_CONF_RETURN_TYPE post_processing_hook_mep(const Action& action) const
    {
        return post_processing_hook_mep(event_set{event<Event>}, action);
    }

    /**
    @brief Specifies the maximum object alignment requirement for the
    run-to-completion event queue to enable small object optimization (and thus
    avoid an extra memory allocation).
    */
    [[nodiscard]] constexpr MAKI_DETAIL_MACHINE_CONF_RETURN_TYPE small_event_max_align(const std::size_t value) const
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
    [[nodiscard]] constexpr MAKI_DETAIL_MACHINE_CONF_RETURN_TYPE small_event_max_size(const std::size_t value) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_small_event_max_size value
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_small_event_max_size
    }

    /**
    @brief Currently doesn't do anything. Once thread safety is implemented,
    will specify whether thread safety is enabled.

    Being a safety option, it will be enabled by default.
    */
    [[nodiscard]] constexpr MAKI_DETAIL_MACHINE_CONF_RETURN_TYPE thread_safe(const bool /*enabled*/) const
    {
        return *this;
    }

    /**
    @brief Specifies the list of transition tables. One region per transition
    table is created.
    */
    template<class... TransitionTables>
    [[nodiscard]] constexpr MAKI_DETAIL_MACHINE_CONF_RETURN_TYPE transition_tables(const TransitionTables&... tables) const
    {
        const auto tpl = detail::tuple<TransitionTables...>{detail::distributed_construct, tables...};
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_transition_tables tpl
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_transition_tables
    }

private:
    using impl_type = Impl;

    template<class Impl2>
    friend class machine_conf;

    template<class... Args>
    constexpr machine_conf(Args&&... args):
        impl_{std::forward<Args>(args)...}
    {
    }

    template<class Context2, auto ContextSig>
    [[nodiscard]] constexpr auto context() const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_context_type detail::type<Context2>
#define MAKI_DETAIL_ARG_context_sig ContextSig
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_context_type
#undef MAKI_DETAIL_ARG_context_sig
    }

    template<action_signature Sig, class EventSetPredicate, class Hook>
    [[nodiscard]] constexpr auto pre_processing_hook(const event_set<EventSetPredicate>& evt_set, const Hook& hook) const
    {
        const auto new_pre_processing_hooks = tuple_append
        (
            impl_.pre_processing_hooks,
            detail::make_event_action<Sig>(evt_set, hook)
        );

        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_pre_processing_hooks new_pre_processing_hooks
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_pre_processing_hooks
    }

#undef MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN

    MAKI_DETAIL_FRIENDLY_IMPL
};

#undef MAKI_DETAIL_MACHINE_CONF_RETURN_TYPE

namespace detail
{
    template<class T>
    struct is_machine_conf
    {
        static constexpr auto value = false;
    };

    template<class... Options>
    struct is_machine_conf<machine_conf<Options...>>
    {
        static constexpr auto value = true;
    };

    template<class T>
    constexpr auto is_machine_conf_v = is_machine_conf<T>::value;
}

} //namespace

#endif
