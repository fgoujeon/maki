//Copyright Florian Goujeon 2021 - 2023.
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

#include "conf_holder.hpp"
#include "machine_conf.hpp"
#include "path.hpp"
#include "detail/noinline.hpp"
#include "detail/submachine.hpp"
#include "detail/function_queue.hpp"
#include "detail/tlu.hpp"
#include "detail/overload_priority.hpp"
#include <type_traits>

namespace maki
{

namespace detail
{
    enum class machine_operation
    {
        start,
        stop,
        process_event
    };
}

/**
@brief The state machine implementation template.
@tparam ConfHolder the state machine definition, a class that must at least define a
`static constexpr auto conf` of a @ref machine_conf type, with defined
machine_conf::transition_tables and machine_conf::context_type options

Here is an example of valid state machine definition, where:
- `transition_table` is a user-provided `constexpr` instance of a @ref
transition_table type;
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
    using conf_type = std::decay_t<decltype(conf)>;

    /**
    @brief The state machine context type.
    */
    using context_type = typename conf_type::context_type;

    static_assert
    (
        detail::is_root_sm_conf_v<std::decay_t<decltype(conf)>>,
        "The root state machine definition must include a 'static constexpr auto conf' of type machine_conf"
    );

    /**
    @brief The constructor.
    @param ctx_args the arguments to be passed to the context constructor

    The constructor first instantiates all the state machine objects (i.e. the
    state machine context, the state machine definition, the region contexts (if
    any) and the states). All state machine objects are constructed with one of
    these statements:
    @code
    auto obj = object_type{*this, context()};
    auto obj = object_type{context()};
    auto obj = object_type{};
    @endcode

    Finally, unless the machine_conf::auto_start is `false`, `start()` is
    called.
    */
    template<class... ContextArgs>
    explicit machine(ContextArgs&&... ctx_args):
        submachine_(*this, std::forward<ContextArgs>(ctx_args)...)
    {
        if constexpr(conf.auto_start_)
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
        return submachine_.context();
    }

    /**
    @brief Returns the context instantiated at construction.
    */
    const context_type& context() const
    {
        return submachine_.context();
    }

    template<const auto& MachineOrStatePath = empty_path_c>
    auto& data()
    {
        return submachine_.template data<MachineOrStatePath>();
    }

    template<const auto& MachineOrStatePath = empty_path_c>
    const auto& data() const
    {
        return submachine_.template data<MachineOrStatePath>();
    }

    /**
    @brief Returns whether the region indicated by `RegionPath` is running.
    @tparam RegionPath an instance of @ref path pointing to the
    region of interest
    */
    template<const auto& RegionPath>
    [[nodiscard]] bool running() const
    {
        return submachine_.template running<RegionPath>();
    }

    /**
    @brief Returns whether the single region of the state machine in running.
    This function can only be called if the state machine contains a single
    region.
    */
    [[nodiscard]] bool running() const
    {
        return submachine_.running();
    }

    /**
    @brief Returns whether `State` is active in the region indicated by
    `RegionPath`.
    @tparam RegionPath an instance of @ref path pointing to the
    region of interest
    @tparam State the state type
    */
    template<const auto& RegionPath, const auto& StateConf>
    [[nodiscard]] bool active_state() const
    {
        return submachine_.template active_state<RegionPath, StateConf>();
    }

    /**
    @brief Returns whether `State` is active in the single region of the state
    machine. This function can only be called if the state machine contains a
    single region.
    @tparam State the state type
    */
    template<const auto& StateConf>
    [[nodiscard]] bool active_state() const
    {
        return submachine_.template active_state<StateConf>();
    }

    /**
    @brief Starts the state machine
    @param event the event to be passed to the event hooks, mainly the
    `on_entry()` function of the initial state(s)

    Concretely, if `maki::state_conf::stopped` is the active state, exits
    `maki::state_conf::stopped` and enters the initial state.

    Reminder: There's no need to call this function after the construction,
    unless machine_conf::auto_start is set to `false`.
    */
    template<class Event = events::start>
    void start(const Event& event = {})
    {
        execute_operation<detail::machine_operation::start>(event);
    }

    /**
    @brief Stops the state machine
    @param event the event to be passed to the event hooks, mainly the
    `on_exit()` function of the active state(s)

    Concretely, if `maki::state_conf::stopped` is not the active state, exits
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
        bool state_transition_happened = false;

        //Process event in transition table
        FOR_EACH_TRANSITION_IN_REGION_TRANSITION_TABLE()
        {
            if
            (
                IS_ACTIVE_STATE(transition_t::source_state_type) &&
                SAME_TYPE(Event, transition_t::event_type) &&
                CALL_GUARD(transition_t)
            )
            {
                CALL_ON_EXIT(transition_t::source_state_type);
                active_state_id = transition_t::target_state_id;
                CALL_ACTION(transition_t);
                CALL_ON_ENTRY(transition_t::target_state_type);

                state_transition_happened = true;
                break;
            }
        }

        //Process event in active state
        if(!state_transition_happened)
        {
            CALL_ON_EVENT(active_state);
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
    @brief Like process_event(), but doesn't check if an event is being
    processed.
    @param event the event to be processed

    <b>USE WITH CAUTION!</b>

    You can call this function if you're **absolutely** sure that you're not
    calling this function while process_event() is being called. Otherwise,
    <b>run-to-completion will be broken</b>.

    Compared to process_event(), process_event_now() is:
    - faster to build, because the enqueue_event() function template won't be
    instantiated;
    - faster to run, because an `if` statement is skipped.
    */
    template<class Event>
    void process_event_now(const Event& event)
    {
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
        submachine_.template call_internal_action<true>(*this, context(), event, processed);
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
        static_assert(conf.run_to_completion_);
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
            conf.small_event_max_size_,
            conf.small_event_max_align_
        >;
    };
    struct empty_holder
    {
        template<bool = true> //Dummy template for lazy evaluation
        struct type{};
    };
    using operation_queue_type = typename std::conditional_t
    <
        conf.run_to_completion_,
        real_operation_queue_holder,
        empty_holder
    >::template type<>;

    template<detail::machine_operation Operation, class Event>
    void execute_operation(const Event& event)
    {
        try
        {
            if constexpr(conf.run_to_completion_)
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
        if constexpr(conf.run_to_completion_)
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
        if constexpr(std::is_same_v<typename conf_type::exception_action_type, detail::noop_ex>)
        {
            process_event(events::exception{eptr});
        }
        else
        {
            conf.exception_action_(*this, eptr);
        }
    }

    template<detail::machine_operation Operation, class Event>
    void execute_one_operation(const Event& event)
    {
        if constexpr(Operation == detail::machine_operation::start)
        {
            submachine_.call_entry_action(*this, context(), event);
        }
        else if constexpr(Operation == detail::machine_operation::stop)
        {
            submachine_.call_exit_action(*this, context(), event);
        }
        else
        {
            if constexpr(std::is_same_v<typename conf_type::fallback_transition_action_tuple_type, detail::tuple<>>)
            {
                submachine_.call_internal_action(*this, context(), event);
            }
            else
            {
                auto processed = false;
                submachine_.call_internal_action(*this, context(), event, processed);
                if(!processed)
                {
                    int dummy_data = 0;
                    call_state_action
                    (
                        conf.fallback_transition_actions_,
                        *this,
                        context(),
                        dummy_data,
                        event
                    );
                }
            }
        }
    }

    detail::submachine<conf, void> submachine_;
    bool executing_operation_ = false;
    operation_queue_type operation_queue_;
};

template<const auto& Conf>
using make_machine = machine<conf_holder<Conf>>;

} //namespace

#endif
