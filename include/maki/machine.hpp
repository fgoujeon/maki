//Copyright Florian Goujeon 2021 - 2024.
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

#include "region_proxy.hpp"
#include "state_proxy.hpp"
#include "conf_holder.hpp"
#include "machine_conf.hpp"
#include "events.hpp"
#include "null.hpp"
#include "detail/path.hpp"
#include "detail/simple_state.hpp" //NOLINT misc-include-cleaner
#include "detail/composite_state.hpp" //NOLINT misc-include-cleaner
#include "detail/composite_state_no_context.hpp"
#include "detail/context_holder.hpp"
#include "detail/event_action.hpp"
#include "detail/noinline.hpp"
#include "detail/function_queue.hpp"
#include "detail/tuple.hpp"
#include <type_traits>
#include <exception>

namespace maki
{

namespace detail
{
    enum class machine_operation: char
    {
        start,
        stop,
        process_event
    };
}

/**
@brief The state machine implementation template.
@tparam ConfHolder the state machine definition, a class that must at least
define a `static constexpr auto conf` of a `maki::machine_conf` type, with
defined `transition_tables` and `context` options

Here is an example of valid state machine definition, where:
- `transition_table` is a user-provided `constexpr` instance of a
`maki::transition_table` type;
- `context` is a user-provided class.

@snippet lamp/src/main.cpp machine-def

The state machine type itself can then be defined like so:
@snippet lamp/src/main.cpp machine
*/
template<class ConfHolder>
class machine
{
public:
    /**
    @brief The state machine configuration.
    */
    static constexpr const auto& conf = ConfHolder::conf;

    /**
    @brief The state machine configuration type.
    */
    using option_set_type = std::decay_t<decltype(opts(conf))>;

    /**
    @brief The state machine context type.
    */
    using context_type = typename option_set_type::context_type;

    static_assert
    (
        detail::is_root_sm_conf_v<std::decay_t<decltype(conf)>>,
        "The root state machine definition must include a 'static constexpr auto conf' of type machine_conf"
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
        ctx_holder_(*this, std::forward<ContextArgs>(ctx_args)...),
        impl_(*this, context())
    {
        if constexpr(opts(conf).auto_start)
        {
            //start
            execute_operation_now<detail::machine_operation::start>(events::start{});
        }
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
        return ctx_holder_.get();
    }

    /**
    @brief Returns the context instantiated at construction.
    */
    const context_type& context() const
    {
        return ctx_holder_.get();
    }

    /**
    @brief Returns whether the region of the state machine in running.
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

    Concretely, if `maki::state_confs::stopped` is the active state, exits
    `maki::state_confs::stopped` and enters the initial state.

    Reminder: There's no need to call this function after the construction,
    unless `maki::machine_conf::auto_start` is set to `false`.
    */
    template<class Event = events::start>
    void start(const Event& event = {})
    {
        execute_operation<detail::machine_operation::start>(event);
    }

    /**
    @brief Stops the state machine
    @param event the event to be passed to the invoked actions, mainly the
    exit action of the active state(s)

    Concretely, if `maki::state_confs::stopped` is not the active state, exits
    the active state and enters `maki::state_confs::stopped`.
    */
    template<class Event = events::stop>
    void stop(const Event& event = {})
    {
        execute_operation<detail::machine_operation::stop>(event);
    }

    /**
    @brief Processes the given event
    @param event the event to be processed

    It's hard to describe all the things this function does, as it is the point
    of the whole library, but let's try to list the basic stuff with the
    following pseudocode:
    @code
    //Run-to-completion: Don't let potential recursive calls interrupt the
    //current processing
    if(processing_event)
    {
        enqueue_event(event);
    }

    //Process the event
    FOR_EACH_REGION()
    {
        //Process event in active state
        const bool processed = CALL_ACTIVE_STATE_INTERNAL_ACTION();

        if(!processed)
        {
            //Process event in transition table
            FOR_EACH_TRANSITION_IN_REGION_TRANSITION_TABLE()
            {
                if
                (
                    IS_ACTIVE_STATE(source_state) &&
                    SAME_TYPE(Event, event_type) &&
                    GUARD() == true
                )
                {
                    CALL_EXIT_ACTION(source_state);
                    SET_ACTIVE_STATE(target_state);
                    CALL_TRANSITION_ACTION();
                    CALL_ENTRY_ACTION(target_state);
                    break;
                }
            }
        }
    }

    //Run-to-completion: Process enqueued events the same way
    process_enqueued_events();
    @endcode
    */
    template<class Event>
    void process_event(const Event& event)
    {
        execute_operation<detail::machine_operation::process_event>(event);
    }

    /**
    @brief Like `maki::machine::process_event()`, but doesn't check if an event
    is being processed.
    @param event the event to be processed

    <b>USE WITH CAUTION!</b>

    You can call this function if you're **absolutely** sure that you're not
    calling this function while `maki::machine::process_event()` is being
    called. Otherwise, <b>run-to-completion will be broken</b>.

    Compared to `maki::machine::process_event()`, this function is:
    - faster to build, because the `maki::machine::enqueue_event()` function
    template won't be instantiated;
    - faster to run, because an `if` statement is skipped.

    `maki::machine_conf::process_event_now_enabled()` must be set to `true` for
    this function to be available.
    */
    template<class Event>
    void process_event_now(const Event& event)
    {
        static_assert
        (
            opts(conf).process_event_now_enabled,
            "`maki::machine_conf::process_event_now_enabled()` hasn't been set to `true`"
        );
        execute_operation_now<detail::machine_operation::process_event>(event);
    }

    /**
    @brief Checks whether calling `process_event(event)` would cause a state
    transition or a call to any action.
    @param event the event to be checked

    This function is useful for checking whether an event is valid or not,
    given the current state of the state machine and guard checks against the
    event itself.

    Note: Run-to-completion mechanism is bypassed.
    */
    template<class Event>
    bool check_event(const Event& event) const
    {
        auto processed = false;
        impl_.template call_internal_action<true>(*this, context(), event, processed);
        return processed;
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
    MAKI_NOINLINE void enqueue_event(const Event& event)
    {
        static_assert(opts(conf).run_to_completion);
        try
        {
            enqueue_event_impl<detail::machine_operation::process_event>(event);
        }
        catch(...)
        {
            process_exception(std::current_exception());
        }
    }

    /**
    @brief Processes events that have been enqueued by the run-to-completion
    mechanism.

    Calling this function is only relevant when managing an exception thrown by
    user code and caught by the state machine.
    */
    void process_enqueued_events()
    {
        if(!executing_operation_)
        {
            auto grd = executing_operation_guard{*this};
            try
            {
                operation_queue_.invoke_and_pop_all(*this);
            }
            catch(...)
            {
                process_exception(std::current_exception());
            }
        }
    }

    template<int Index>
    [[nodiscard]] auto region() const
    {
        return detail::make_region_proxy(impl_.template region<Index>());
    }

    template<const auto& StateConf>
    [[nodiscard]] auto state() const
    {
        return detail::make_state_proxy(impl_.template state<StateConf>());
    }

    /**
    @brief Returns whether the state created from `StateConf` is active in the
    region of the state machine. This function can only be called if the state
    machine contains a single region.
    @tparam StateConf the state configurator
    */
    template<const auto& StateConf>
    [[nodiscard]] bool is() const
    {
        return impl_.template is<StateConf>();
    }

private:
    class executing_operation_guard
    {
    public:
        executing_operation_guard(machine& self):
            self_(self)
        {
            self_.executing_operation_ = true;
        }

        executing_operation_guard(const executing_operation_guard&) = delete;
        executing_operation_guard(executing_operation_guard&&) = delete;
        executing_operation_guard& operator=(const executing_operation_guard&) = delete;
        executing_operation_guard& operator=(executing_operation_guard&&) = delete;

        ~executing_operation_guard()
        {
            self_.executing_operation_ = false;
        }

    private:
        machine& self_; //NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    };

    struct real_operation_queue_holder
    {
        template<bool = true> //Dummy template for lazy evaluation
        using type = detail::function_queue
        <
            machine&,
            opts(conf).small_event_max_size,
            opts(conf).small_event_max_align
        >;
    };
    struct empty_holder
    {
        template<bool = true> //Dummy template for lazy evaluation
        struct type{};
    };
    using operation_queue_type = typename std::conditional_t
    <
        opts(conf).run_to_completion,
        real_operation_queue_holder,
        empty_holder
    >::template type<>;

    template<detail::machine_operation Operation, class Event>
    void execute_operation(const Event& event)
    {
        try
        {
            if constexpr(opts(conf).run_to_completion)
            {
                if(!executing_operation_) //If call is not recursive
                {
                    execute_operation_now<Operation>(event);
                }
                else
                {
                    //Enqueue event in case of recursive call
                    enqueue_event_impl<Operation>(event);
                }
            }
            else
            {
                execute_one_operation<Operation>(event);
            }
        }
        catch(...)
        {
            process_exception(std::current_exception());
        }
    }

    template<detail::machine_operation Operation, class Event>
    void execute_operation_now(const Event& event)
    {
        if constexpr(opts(conf).run_to_completion)
        {
            auto grd = executing_operation_guard{*this};

            execute_one_operation<Operation>(event);

            //Process enqueued events, if any
            operation_queue_.invoke_and_pop_all(*this);
        }
        else
        {
            execute_one_operation<Operation>(event);
        }
    }

    template<detail::machine_operation Operation, class Event>
    void enqueue_event_impl(const Event& event)
    {
        operation_queue_.template push<any_event_visitor<Operation>>(event);
    }

    template<detail::machine_operation Operation>
    struct any_event_visitor
    {
        template<class Event>
        static void call(const Event& event, machine& self)
        {
            self.execute_one_operation<Operation>(event);
        }
    };

    void process_exception(const std::exception_ptr& eptr)
    {
        if constexpr(std::is_same_v<typename option_set_type::exception_hook_type, null_t>)
        {
            process_event(events::exception{eptr});
        }
        else
        {
            opts(conf).exception_hook(*this, eptr);
        }
    }

    template<detail::machine_operation Operation, class Event>
    void execute_one_operation(const Event& event)
    {
        if constexpr(Operation == detail::machine_operation::start)
        {
            impl_.call_entry_action(*this, context(), event);
        }
        else if constexpr(Operation == detail::machine_operation::stop)
        {
            impl_.call_exit_action(*this, context(), event);
        }
        else
        {
            if constexpr(std::is_same_v<typename option_set_type::post_processing_hook_tuple_type, detail::tuple<>>)
            {
                impl_.call_internal_action(*this, context(), event);
            }
            else
            {
                auto processed = false;
                impl_.call_internal_action(*this, context(), event, processed);
                detail::call_matching_event_action<post_processing_hook_ptr_constant_list>
                (
                    *this,
                    context(),
                    event,
                    processed
                );
            }
        }
    }

    static constexpr auto post_processing_hooks = opts(conf).post_processing_hooks;
    static constexpr auto path = detail::path{}.add_state<conf>();

    using post_processing_hook_ptr_constant_list = detail::tuple_to_element_ptr_constant_list_t<post_processing_hooks>;

    detail::context_holder<context_type, opts(conf).context_sig> ctx_holder_;
    detail::composite_state_no_context<&conf, path> impl_;
    bool executing_operation_ = false;
    operation_queue_type operation_queue_;
};

template<const auto& Conf>
using make_machine = machine<conf_holder<Conf>>;

} //namespace

#endif
