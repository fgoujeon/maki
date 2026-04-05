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

#include "context.hpp"
#include "detail/context_holder.hpp"
#include "detail/context_storage.hpp"
#include "detail/event_action.hpp"
#include "detail/friendly_impl.hpp"
#include "detail/function_queue.hpp"
#include "detail/integer_constant_sequence.hpp"
#include "detail/mix.hpp"
#include "detail/noinline.hpp"
#include "detail/path_impl.hpp"
#include "detail/region_impl.hpp"
#include "detail/state_impls/simple.hpp" //NOLINT misc-include-cleaner
#include "detail/state_impls/simple_no_context.hpp"
#include "detail/tlu/apply.hpp"
#include "detail/tlu/contains_if.hpp"
#include "detail/tlu/for_each.hpp"
#include "detail/tlu/for_each_plus.hpp"
#include "detail/tlu/get.hpp"
#include "detail/tlu/left_fold.hpp"
#include "detail/tlu/size.hpp"
#include "detail/type_set.hpp"
#include "events.hpp"
#include "machine_conf.hpp"
#include "null.hpp"
#include "region.hpp"
#include "states.hpp"
#include <exception>
#include <type_traits>
#include <utility>

namespace maki
{

namespace detail
{
    template
    <
        class ParentSm,
        const auto& ParentPath,
        context_storage ParentCtxStorage,
        int Index
    >
    struct region_mix_elem
    {
        static constexpr auto transition_tbl = tuple_get<Index>(impl_of(ParentSm::conf).transition_tables);
        static constexpr auto path = ParentPath.add_region_index(Index);
        using type = region<region_impl<transition_tbl, path, ParentCtxStorage>>;
    };

    template
    <
        class ParentSm,
        const auto& ParentPath,
        context_storage ParentCtxStorage,
        int Index
    >
    using region_mix_elem_t = typename region_mix_elem<ParentSm, ParentPath, ParentCtxStorage, Index>::type;

    template
    <
        class ParentSm,
        const auto& ParentPath,
        context_storage ParentCtxStorage,
        class RegionIndexSequence
    >
    struct region_mix;

    template
    <
        class ParentSm,
        const auto& ParentPath,
        context_storage ParentCtxStorage,
        int... RegionIndexes
    >
    struct region_mix
    <
        ParentSm,
        ParentPath,
        ParentCtxStorage,
        std::integer_sequence<int, RegionIndexes...>
    >
    {
        using type = mix
        <
            region_mix_elem_t
            <
                ParentSm,
                ParentPath,
                ParentCtxStorage,
                RegionIndexes
            >...
        >;
    };

    template<class EventTypeSet, class Region>
    using region_type_list_event_type_set_operation =
        type_set_union_t
        <
            EventTypeSet,
            typename impl_of_t<Region>::event_type_set
        >
    ;

    template<class RegionTypeList>
    using region_type_list_event_type_set = tlu::left_fold_t
    <
        RegionTypeList,
        region_type_list_event_type_set_operation,
        empty_type_set_t
    >;

    enum class machine_operation: char
    {
        start,
        stop,
        process_event
    };
}

#define MAKI_DETAIL_MAYBE_CATCH(statement) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    if constexpr(detail::is_null_v<typename option_set_type::exception_handler_type>) \
    { \
        statement; \
    } \
    else \
    { \
        try \
        { \
            statement; \
        } \
        catch(...) \
        { \
            impl_of(conf).exception_handler(*this, std::current_exception()); \
        } \
    }

/**
@brief The state machine implementation template.
@tparam Conf the state machine configuration, with defined `transition_tables`
and `context` options

Here is an example of valid state machine definition, where:
- `transition_table` is a user-provided `constexpr` instance of a
`maki::transition_table` type;
- `context` is a user-provided class.

@snippet concepts/state-machine/src/main.cpp machine-def

The state machine type itself can then be defined like so:
@snippet concepts/state-machine/src/main.cpp machine
*/
template<const auto& Conf>
class machine
{
public:
    /**
    @brief The state machine configuration.
    */
    static constexpr const auto& conf = Conf;

#ifndef MAKI_DETAIL_DOXYGEN
    using option_set_type = std::decay_t<decltype(impl_of(conf))>;
#endif

#ifdef MAKI_DETAIL_DOXYGEN
    /**
    @brief The context type given to `maki::machine_conf::context_a()` or its variants.
    */
    using context_type = IMPLEMENTATION_DETAIL;
#else
    using context_type = typename option_set_type::context_type;
#endif

    static_assert
    (
        detail::is_machine_conf_v<std::decay_t<decltype(conf)>>,
        "Given `Conf` must be an instance of `maki::machine_conf`"
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
        regions_(detail::mix_uniform_construct, *this, context())
    {
        if constexpr(impl_of(conf).auto_start)
        {
            MAKI_DETAIL_MAYBE_CATCH(start_now())
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
    @brief Returns whether the region of the state machine is running.
    This function can only be called if the state machine contains only one
    region.
    */
    [[nodiscard]] bool running() const
    {
        return !completed();
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
        MAKI_DETAIL_MAYBE_CATCH(start_no_catch(event))
    }

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
        MAKI_DETAIL_MAYBE_CATCH(stop_no_catch(event))
    }

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
        MAKI_DETAIL_MAYBE_CATCH(process_event_no_catch(event))
    }

    /**
    @brief Like `process_event()`, but doesn't catch exceptions, even if
    `maki::machine_conf::catch_mx()` is set.
    */
    template<class Event>
    void process_event_no_catch(const Event& event)
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
    - faster to build, because the `maki::machine::push_event()` function
    template won't be instantiated;
    - faster to run, because an `if` statement is skipped.

    `maki::machine_conf::process_event_now_enabled()` must be set to `true` for
    this function to be available.
    */
    template<class Event>
    void process_event_now(const Event& event)
    {
        MAKI_DETAIL_MAYBE_CATCH(process_event_now_no_catch(event))
    }

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
        return call_internal_action<true>(*this, context(), event);
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
        MAKI_DETAIL_MAYBE_CATCH(push_event_no_catch(event))
    }

    /**
    @brief Returns the `maki::region` object at index `Index`.
    */
    template<int Index>
    [[nodiscard]] const auto& region() const
    {
        using region_type = detail::tlu::get_t<region_mix_type, Index>;
        return detail::get<region_type>(regions_);
    }

    /**
    @brief Returns the `maki::state` object created by `StateMold` (of type
    `maki::state_mold`). Only valid if machine is only made of one region.
    */
    template<const auto& StateMold>
    [[nodiscard]] const auto& state() const
    {
        static_assert(region_mix_type::size == 1);
        return impl_of(region<0>()).template state<StateMold>();
    }

    /**
    @brief Returns whether the state created by `StateMold` is active in the
    region of the state machine. Only valid if machine is only made of one
    region.
    */
    template<const auto& StateMold>
    [[nodiscard]] bool is() const
    {
        static_assert(region_mix_type::size == 1);
        return impl_of(region<0>()).template is<StateMold>();
    }

private:
    using transition_table_type_list = decltype(impl_of(conf).transition_tables);
    using impl_type = detail::state_impls::simple_no_context<&conf>;

    static constexpr auto ctx_lifetime = impl_of(conf).context_lifetime;

    static constexpr auto ctx_storage = detail::context_storage::plain;

    static constexpr auto path = detail::path_impl{};

    using region_index_sequence_type = std::make_integer_sequence
    <
        int,
        detail::tlu::size_v<transition_table_type_list>
    >;

    using region_index_constant_sequence = detail::make_integer_constant_sequence
    <
        int,
        detail::tlu::size_v<transition_table_type_list>
    >;

    using region_mix_type = typename detail::region_mix
    <
        machine,
        path,
        ctx_storage,
        region_index_sequence_type
    >::type;

    using event_type_set = detail::type_set_union_t
    <
        typename impl_type::event_type_set,
        detail::region_type_list_event_type_set<region_mix_type>
    >;

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
            impl_of(conf).small_event_max_size,
            impl_of(conf).small_event_max_align
        >;
    };
    struct empty_holder
    {
        template<bool = true> //Dummy template for lazy evaluation
        struct type{};
    };
    using operation_queue_type = typename std::conditional_t
    <
        impl_of(conf).run_to_completion,
        real_operation_queue_holder,
        empty_holder
    >::template type<>;

    void start_now()
    {
        execute_operation_now<detail::machine_operation::start>(events::start{});
    }

    template<class Event>
    void start_no_catch(const Event& event)
    {
        if(!running())
        {
            execute_operation<detail::machine_operation::start>(event);
        }
    }

    template<class Event>
    void stop_no_catch(const Event& event)
    {
        if(running())
        {
            execute_operation<detail::machine_operation::stop>(event);
        }
    }

    template<class Event>
    void process_event_now_no_catch(const Event& event)
    {
        static_assert
        (
            impl_of(conf).process_event_now_enabled,
            "`maki::machine_conf::process_event_now_enabled()` hasn't been set to `true`"
        );
        execute_operation_now<detail::machine_operation::process_event>(event);
    }

    template<detail::machine_operation Operation, class Event>
    void execute_operation(const Event& event)
    {
        if constexpr(impl_of(conf).run_to_completion)
        {
            if(!executing_operation_) //If call is not recursive
            {
                execute_operation_now<Operation>(event);
            }
            else
            {
                //Push event to RTC queue in case of recursive call
                push_event_impl<Operation>(event);
            }
        }
        else
        {
            execute_one_operation<Operation>(event);
        }
    }

    template<detail::machine_operation Operation, class Event>
    void execute_operation_now(const Event& event)
    {
        if constexpr(impl_of(conf).run_to_completion)
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

    template<class Event>
    MAKI_NOINLINE void push_event_no_catch(const Event& event)
    {
        static_assert(impl_of(conf).run_to_completion);
        push_event_impl<detail::machine_operation::process_event>(event);
    }

    template<detail::machine_operation Operation, class Event>
    void push_event_impl(const Event& event)
    {
        operation_queue_.template push<any_event_visitor<Operation>>(event);
    }

    /**
    @brief Processes events that have been pushed into the run-to-completion
    queue.

    Calling this function is only relevant when managing an exception thrown by
    user code and caught by the state machine.
    */
    void process_pending_events()
    {
        if(!executing_operation_)
        {
            auto grd = executing_operation_guard{*this};
            operation_queue_.invoke_and_pop_all(*this);
        }
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

    template<detail::machine_operation Operation, class Event>
    void execute_one_operation(const Event& event)
    {
        if constexpr(Operation == detail::machine_operation::start)
        {
            enter(*this, context(), event);
        }
        else if constexpr(Operation == detail::machine_operation::stop)
        {
            exit_to_finals(*this, context(), event);
        }
        else
        {
            constexpr auto has_matching_pre_processing_hook = detail::tlu::contains_if_v
            <
                pre_processing_hook_ptr_constant_list,
                detail::event_action_traits::for_event<Event>::template has_containing_event_set
            >;

            constexpr auto has_matching_post_processing_hook = detail::tlu::contains_if_v
            <
                post_processing_hook_ptr_constant_list,
                detail::event_action_traits::for_event<Event>::template has_containing_event_set
            >;

            //If running, execute pre-processing hook for `Event`, if any.
            if constexpr(has_matching_pre_processing_hook)
            {
                if(running())
                {
                    detail::call_matching_event_action<pre_processing_hook_ptr_constant_list>
                    (
                        *this,
                        context(),
                        event
                    );
                }
            }

            /*
            If running:
            - process the event;
            - execute the post-processing hook for `Event`, if any.
            */
            if constexpr(has_matching_post_processing_hook)
            {
                if(running())
                {
                    const auto processed = call_internal_action<false>(*this, context(), event);
                    detail::call_matching_event_action<post_processing_hook_ptr_constant_list>
                    (
                        *this,
                        context(),
                        event,
                        processed
                    );
                }
            }
            else
            {
                /*
                Note: We don't need to check if we're running here, as
                processing the event won't have any effect anyway if the machine
                is stopped.
                */

                call_internal_action<false>(*this, context(), event);
            }
        }
    }

    template<class Context, class Machine>
    void emplace_contexts_with_parent_lifetime(Context& ctx, Machine& mach)
    {
        detail::tlu::for_each
        <
            region_mix_type,
            region_emplace_contexts_with_parent_lifetime
        >(*this, ctx, mach);
    }

    template<class Machine, class Context, class Event>
    void enter(Machine& mach, Context& ctx, const Event& event)
    {
        impl_.enter(mach, ctx, event);
        detail::tlu::for_each<region_mix_type, region_enter>(*this, mach, ctx, event);
    }

    template<bool Dry, class Machine, class Context, class Event>
    bool call_internal_action
    (
        Machine& mach,
        Context& ctx,
        const Event& event
    )
    {
        return call_internal_action_2<Dry>(*this, mach, ctx, event);
    }

    template<bool Dry, class Machine, class Context, class Event>
    bool call_internal_action
    (
        Machine& mach,
        Context& ctx,
        const Event& event
    ) const
    {
        return call_internal_action_2<Dry>(*this, mach, ctx, event);
    }

    template<class Machine, class Context, class Event>
    void exit(Machine& mach, Context& ctx, const Event& event)
    {
        detail::tlu::for_each<region_mix_type, region_exit<&detail::state_molds::null>>
        (
            *this,
            mach,
            ctx,
            event
        );
        impl_.exit
        (
            mach,
            ctx,
            event
        );
    }

    // For each region, transition from active state to final state.
    template<class Machine, class Context, class Event>
    void exit_to_finals(Machine& mach, Context& ctx, const Event& event)
    {
        detail::tlu::for_each<region_mix_type, region_exit<&detail::state_molds::fin>>
        (
            *this,
            mach,
            ctx,
            event
        );
        impl_.exit
        (
            mach,
            ctx,
            event
        );
    }

    void reset_contexts_with_parent_lifetime()
    {
        detail::tlu::for_each
        <
            region_mix_type,
            region_reset_contexts_with_parent_lifetime
        >(*this);
    }

    [[nodiscard]] bool completed() const
    {
        return detail::tlu::apply_t
        <
            region_mix_type,
            all_regions_completed
        >::call(*this);
    }

    template<class... Regions>
    struct all_regions_completed
    {
        template<class Self>
        static bool call(const Self& self)
        {
            return (impl_of(detail::get<Regions>(self.regions_)).completed() && ...);
        }
    };

    struct region_emplace_contexts_with_parent_lifetime
    {
        template<class Region, class Self, class Context, class Machine>
        static void call(Self& self, Context& ctx, Machine& mach)
        {
            impl_of(detail::get<Region>(self.regions_)).emplace_contexts_with_parent_lifetime(ctx, mach);
        }
    };

    struct region_enter
    {
        template<class Region, class Self, class Machine, class Context, class Event>
        static void call(Self& self, Machine& mach, Context& ctx, const Event& event)
        {
            impl_of(detail::get<Region>(self.regions_)).enter(mach, ctx, event);
        }
    };

    template<bool Dry>
    struct region_process_event
    {
        template<class Region, class Self, class Machine, class Context, class Event>
        static int call(Self& self, Machine& mach, Context& ctx, const Event& event)
        {
            const auto processed = impl_of(detail::get<Region>(self.regions_)).template process_event<Dry>(mach, ctx, event);
            return static_cast<int>(processed);
        }
    };

    template<auto TargetStateId>
    struct region_exit
    {
        template<class Region, class Self, class Machine, class Context, class Event>
        static void call(Self& self, Machine& mach, Context& ctx, const Event& event)
        {
            impl_of(detail::get<Region>(self.regions_)).template exit<TargetStateId>(mach, ctx, event);
        }
    };

    struct region_reset_contexts_with_parent_lifetime
    {
        template<class Region, class Self>
        static void call(Self& self)
        {
            impl_of(detail::get<Region>(self.regions_)).reset_contexts_with_parent_lifetime();
        }
    };

    template<bool Dry, class Self, class Machine, class Context, class Event>
    static bool call_internal_action_2
    (
        Self& self,
        Machine& mach,
        Context& ctx,
        const Event& event
    )
    {
        constexpr auto can_process_event = detail::type_set_contains_v
        <
            typename impl_type::event_type_set,
            Event
        >;

        if constexpr(can_process_event)
        {
            self.impl_.template call_internal_action<Dry>
            (
                mach,
                ctx,
                event
            );

            detail::tlu::for_each<region_mix_type, region_process_event<Dry>>(self, mach, ctx, event);

            return true;
        }
        else
        {
            const auto processed_count = detail::tlu::for_each_plus<region_mix_type, region_process_event<Dry>>(self, mach, ctx, event);
            return static_cast<bool>(processed_count);
        }
    }

    static constexpr auto pre_processing_hooks = impl_of(conf).pre_processing_hooks;
    static constexpr auto post_processing_hooks = impl_of(conf).post_processing_hooks;

    using pre_processing_hook_ptr_constant_list = detail::mix_constant_list_t<pre_processing_hooks>;
    using post_processing_hook_ptr_constant_list = detail::mix_constant_list_t<post_processing_hooks>;

    detail::context_holder
    <
        context_type,
        detail::context_storage::plain,
        impl_of(conf).context_sig
    > ctx_holder_;
    impl_type impl_;
    region_mix_type regions_;
    bool executing_operation_ = false;
    operation_queue_type operation_queue_;
};

#undef MAKI_DETAIL_MAYBE_CATCH

} //namespace

#endif
