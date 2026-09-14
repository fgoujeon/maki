//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::machine class template
*/

#ifndef MAKI_MACHINE_HPP
#define MAKI_MACHINE_HPP

#include "machine_conf.hpp"
#include "events.hpp"
#include "null.hpp"
#include "detail/machine_impl.hpp"
#include "detail/machine_operation.hpp"
#include <type_traits>
#include <exception>

namespace maki
{

/**
@brief The state machine implementation template.
@tparam MachineConfHolder the state machine configuration, with defined `transition_tables`
and `context` options

Here is an example of valid state machine definition, where:
- `transition_table` is a user-provided `constexpr` instance of a
`maki::transition_table` type;
- `context` is a user-provided class.

@snippet concepts/state-machine/src/main.cpp machine-def

The state machine type itself can then be defined like so:
@snippet concepts/state-machine/src/main.cpp machine
*/
template<class MachineConfHolder>
class machine
{
public:
    /**
    @brief The state machine configuration.
    */
    static constexpr const auto& conf = MachineConfHolder::value;

#ifndef MAKI_DETAIL_DOXYGEN
    using conf_type = std::decay_t<decltype(detail::impl_of(conf))>;

    static constexpr auto sync = std::is_void_v<typename conf_type::awaitable_template_holder>;

#if __cpp_impl_coroutine
    struct sync_impl_type_holder
    {
        template<bool = true> //Dummy template for lazy evaluation
        using type = detail::machine_impl<MachineConfHolder>;
    };

    struct async_impl_type_holder
    {
        template<bool = true> //Dummy template for lazy evaluation
        using type = detail::async_machine_impl<MachineConfHolder>;
    };

    using impl_type = typename std::conditional_t
    <
        sync,
        sync_impl_type_holder,
        async_impl_type_holder
    >::template type<>;
#else
    using impl_type = detail::machine_impl<MachineConfHolder>;
#endif
#endif

#ifdef MAKI_DETAIL_DOXYGEN
    /**
    @brief The context type given to `maki::machine_conf::context_a()` or its variants.
    */
    using context_type = IMPLEMENTATION_DETAIL;
#else
    using context_type = typename conf_type::context_type;
#endif

#if __cpp_impl_coroutine
#ifdef MAKI_DETAIL_DOXYGEN
    /**
    @brief The awaitable type template given to `maki::machine_conf::async()`.
    */
    template<class T>
    using awaitable_type = IMPLEMENTATION_DETAIL;
#else
    template<class T>
    using awaitable_type = impl_type::template aos_type<T>;
#endif
#endif

    static_assert
    (
        detail::is_machine_conf_v<std::decay_t<decltype(conf)>>,
        "Type of given `MachineConfHolder::value` must be an instance of `maki::machine_conf`"
    );

    static_assert
    (
        !(detail::impl_of(conf).auto_start && !sync),
        "Auto-start must be disabled in asynchronous mode"
    );

    /**
    @brief The constructor.
    @param ctx_args the arguments to be forwarded to the constructor of the root
    context

    The constructor first instantiates all the contexts defined in the state
    machine, starting with the root context (i.e. the context specified in the
    `maki::machine_conf` object).

    Finally, unless `maki::machine_conf::auto_start()` is set to `false`,
    `maki::machine::start()` is called.
    */
    template<class... ContextArgs>
    explicit machine(ContextArgs&&... ctx_args):
        impl_(*this, std::forward<ContextArgs>(ctx_args)...)
    {
    }

    machine(const machine&) = delete;
    machine(machine&&) = delete;
    machine& operator=(const machine&) = delete;
    machine& operator=(machine&&) = delete;
    ~machine() = default;

    /**
    @brief Returns the context instantiated at construction.
    */
    context_type& context()
    {
        return impl_.context();
    }

    /**
    @brief Returns the context instantiated at construction.
    */
    const context_type& context() const
    {
        return impl_.context();
    }

    /**
    @brief Returns whether the region of the state machine is running.
    This function can only be called if the state machine contains only one
    region.
    */
    [[nodiscard]] bool running() const
    {
        return impl_.running();
    }

    /**
    @brief Starts the state machine
    @param event the event to be passed to the invoked actions, mainly the
    entry action of the initial state(s)

    Concretely, if the machine is not already running, exits
    the internal `stopped` state and enters the initial state.

    Reminder: There's no need to call this function after the construction,
    unless `maki::machine_conf::auto_start` is set to `false`.
    */
    template<class Event = events::start>
    void start(const Event& event = {})
    {
        static_assert(sync, "Only available in synchronous mode");
        impl_.start(*this, event);
    }

#ifdef __cpp_impl_coroutine
    template<class Event = events::start>
    awaitable_type<void> async_start(const Event event = {})
    {
        static_assert(!sync, "Only available in asynchronous mode");
        co_await impl_.start(*this, event);
    }
#endif

    /**
    @brief Stops the state machine
    @param event the event to be passed to the invoked actions, mainly the
    exit action of the active state(s)

    Concretely, if the machine is running, exits the active state and enters
    the internal `stopped` state.
    */
    template<class Event = events::stop>
    void stop(const Event& event = {})
    {
        static_assert(sync, "Only available in synchronous mode");
        impl_.stop(*this, event);
    }

#ifdef __cpp_impl_coroutine
    template<class Event = events::stop>
    awaitable_type<void> async_stop(const Event event = {})
    {
        static_assert(!sync, "Only available in asynchronous mode");
        co_await impl_.stop(*this, event);
    }
#endif

    /**
    @brief Processes the given event
    @param event the event to be processed

    It's hard to describe all the things this function does, as it is the point
    of the whole library, but let's try to list the basic stuff with the
    following pseudocode:
    @code
    //Run-to-completion: Don't let potential recursive calls interrupt the
    //current processing.
    if(processing_event)
    {
        push_event(event);
        return;
    }

    //Process the event.
    FOR_EACH_REGION()
    {
        //Process event in active state.
        const bool processed = CALL_ACTIVE_STATE_INTERNAL_ACTION();

        if(!processed)
        {
            //Process event in transition table.
            FOR_EACH_TRANSITION_IN_REGION_TRANSITION_TABLE()
            {
                if
                (
                    IS_ACTIVE_STATE(source_state) &&
                    SAME_TYPE(Event, event_type) &&
                    GUARD() == true
                )
                {
                    SET_ACTIVE_STATE(maki::undefined);
                    CALL_EXIT_ACTION(source_state);
                    CALL_TRANSITION_ACTION();
                    CALL_ENTRY_ACTION(target_state);
                    SET_ACTIVE_STATE(target_state);
                    break;
                }
            }
        }
    }

    //Run-to-completion: Process pending events the same way
    process_pending_events();
    @endcode
    */
    template<class Event>
    void process_event(const Event& event)
    {
        static_assert(sync, "Only available in synchronous mode");
        impl_.process_event(*this, event);
    }

#ifdef __cpp_impl_coroutine
    template<class Event>
    awaitable_type<void> async_process_event(const Event event)
    {
        static_assert(!sync, "Only available in asynchronous mode");
        co_await impl_.process_event(*this, event);
    }
#endif

    /**
    @brief Like `process_event()`, but doesn't catch exceptions, even if
    `maki::machine_conf::catch_mx()` is set.
    */
    template<class Event>
    void process_event_no_catch(const Event& event)
    {
        static_assert(sync, "Only available in synchronous mode");
        impl_.process_event_no_catch(*this, event);
    }

#ifdef __cpp_impl_coroutine
    template<class Event>
    awaitable_type<void> async_process_event_no_catch(const Event event)
    {
        static_assert(!sync, "Only available in asynchronous mode");
        co_await impl_.process_event_no_catch(*this, event);
    }
#endif

    /**
    @brief Like `maki::machine::process_event()`, but doesn't check if an event
    is being processed.
    @param event the event to be processed

    <b>USE WITH CAUTION!</b>

    You can call this function if you're **absolutely** sure that you're not
    calling this function while `maki::machine::process_event()` is being
    called. Otherwise, <b>run-to-completion will be broken</b>.

    Compared to `maki::machine::process_event()`, this function is:
    - faster to build, because the `maki::machine::push_event()` function
    template won't be instantiated;
    - faster to run, because an `if` statement is skipped.

    `maki::machine_conf::process_event_now_enabled()` must be set to `true` for
    this function to be available.
    */
    template<class Event>
    void process_event_now(const Event& event)
    {
        static_assert(sync, "Only available in synchronous mode");
        impl_.process_event_now(*this, event);
    }

#ifdef __cpp_impl_coroutine
    template<class Event>
    awaitable_type<void> async_process_event_now(const Event event)
    {
        static_assert(!sync, "Only available in asynchronous mode");
        co_await impl_.process_event_now(*this, event);
    }
#endif

    /**
    @brief Checks whether calling `process_event(event)` would cause a state
    transition or a call to any action.
    @param event the event to be checked

    This function is useful for checking whether an event is valid or not,
    given the current state of the state machine and guard checks against the
    event itself.

    Note: Run-to-completion mechanism is bypassed and exceptions are not caught.
    */
    template<class Event>
    bool check_event(const Event& event) const
    {
        return impl_.check_event(*this, event);
    }

    /**
    @brief Enqueues event for later processing
    @param event the event to be processed

    You can call this function instead of doing a recursive call to @ref
    process_event().

    This function is slightly faster than @ref process_event(), but if you're
    not sure what you're doing, just call @ref process_event() instead.
    */
    template<class Event>
    MAKI_NOINLINE void push_event(const Event& event)
    {
        impl_.push_event(event);
    }

    /**
    @brief Returns the `maki::region` object at index `Index`.
    */
    template<int Index>
    [[nodiscard]] const auto& region() const
    {
        return impl_.template region<Index>();
    }

    /**
    @brief Returns the `maki::state` object created by `StateMold` (of type
    `maki::state_mold`). Only valid if machine is only made of one region.
    */
    template<const auto& StateMold>
    [[nodiscard]] const auto& state() const
    {
        return impl_.template state<StateMold>();
    }

    /**
    @brief Returns whether the state created by `StateMold` is active in the
    region of the state machine. Only valid if machine is only made of one
    region.
    */
    template<const auto& StateMold>
    [[nodiscard]] bool is() const
    {
        return impl_.template is<StateMold>();
    }

private:
    MAKI_DETAIL_FRIENDLY_IMPL

    impl_type impl_;
};

} //namespace

#endif
