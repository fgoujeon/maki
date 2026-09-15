//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

namespace maki::detail
{

#define MAKI_DETAIL_MAYBE_CATCH(statements) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    if constexpr(detail::is_null_v<typename conf_type::exception_handler_type>) \
    { \
        statements \
    } \
    else \
    { \
        try \
        { \
            statements \
        } \
        catch(...) \
        { \
            detail::impl_of(conf).exception_handler(mach, std::current_exception()); \
        } \
    }

template<class MachineConfHolder>
class MAKI_AOS(machine_impl)
{
public:
    static constexpr const auto& conf = MachineConfHolder::value;

    using conf_type = std::decay_t<decltype(detail::impl_of(conf))>;

    using context_type = typename conf_type::context_type;

#if MAKI_AOS_ASYNC
    template<class T>
    using awaitable_type = typename conf_type::awaitable_template_holder::template type<T>;
#else
    template<class T>
    using awaitable_type = T;
#endif

    template<class T>
    using aos_type = awaitable_type<T>;

    template<class... ContextArgs>
    MAKI_AOS(machine_impl)
    (
        machine<MachineConfHolder>& mach,
        ContextArgs&&... ctx_args
    ):
        ctx_holder_(mach, std::forward<ContextArgs>(ctx_args)...),
        impl_(mach, ctx_holder_.get())
    {
#if !MAKI_AOS_ASYNC
        if constexpr(detail::impl_of(conf).auto_start)
        {
            MAKI_DETAIL_MAYBE_CATCH
            (
                start_now(mach);
            )
        }
#endif
    }

    context_type& context()
    {
        return ctx_holder_.get();
    }

    const context_type& context() const
    {
        return ctx_holder_.get();
    }

    [[nodiscard]] bool running() const
    {
        return !impl_.completed();
    }

    template<class Event = events::start>
    MAKI_AOS_TYPE(void) start
    (
        machine<MachineConfHolder>& mach,
        const Event& event = {}
    )
    {
        MAKI_DETAIL_MAYBE_CATCH
        (
            MAKI_AOS_CALL start_no_catch(mach, event);
        )
    }

    template<class Event = events::stop>
    MAKI_AOS_TYPE(void) stop
    (
        machine<MachineConfHolder>& mach,
        const Event& event = {}
    )
    {
        MAKI_DETAIL_MAYBE_CATCH
        (
            MAKI_AOS_CALL stop_no_catch(mach, event);
        )
    }

    template<class Event>
    MAKI_AOS_TYPE(void) process_event
    (
        machine<MachineConfHolder>& mach,
        const Event& event
    )
    {
        MAKI_DETAIL_MAYBE_CATCH
        (
            MAKI_AOS_CALL process_event_no_catch(mach, event);
        )
    }

    template<class Event>
    MAKI_AOS_TYPE(void) process_event_no_catch
    (
        machine<MachineConfHolder>& mach,
        const Event& event
    )
    {
        MAKI_AOS_CALL execute_operation<detail::machine_operation::process_event>(mach, event);
    }

    template<class Event>
    MAKI_AOS_TYPE(void) process_event_now
    (
        machine<MachineConfHolder>& mach,
        const Event& event
    )
    {
        MAKI_DETAIL_MAYBE_CATCH
        (
            MAKI_AOS_CALL process_event_now_no_catch(mach, event);
        )
    }

#if !MAKI_AOS_ASYNC
    template<class Event>
    bool check_event
    (
        const machine<MachineConfHolder>& mach,
        const Event& event
    ) const
    {
        return impl_.template MAKI_AOS(call_internal_action)<aos_type, true>(mach, context(), event);
    }
#endif

    template<class Event>
    MAKI_NOINLINE void push_event
    (
        machine<MachineConfHolder>& mach,
        const Event& event
    )
    {
        MAKI_DETAIL_MAYBE_CATCH
        (
            push_event_no_catch(mach, event);
        )
    }

    template<int Index>
    [[nodiscard]] const auto& region() const
    {
        return impl_.template region<Index>();
    }

    template<const auto& StateMold>
    [[nodiscard]] const auto& state() const
    {
        return impl_.template state<StateMold>();
    }

    template<const auto& StateMold>
    [[nodiscard]] bool is() const
    {
        return impl_.template is<StateMold>();
    }

private:
    using impl_type =
        detail::state_impls::MAKI_AOS(composite_no_context)
        <
            MachineConfHolder,
            detail::iseq<>,
            detail::context_storage::plain
        >
    ;

    using deferrable_event_type_set =
        typename impl_type::deferrable_event_type_set
    ;

    static constexpr bool has_deferrable_events =
        !detail::type_set_empty_v<deferrable_event_type_set>
    ;

    class executing_operation_guard
    {
    public:
        executing_operation_guard(MAKI_AOS(machine_impl)& self):
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
        MAKI_AOS(machine_impl)& self_; //NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    };

    struct real_function_queue_holder
    {
        template<bool = true> //Dummy template for lazy evaluation
        using type = detail::MAKI_AOS(function_queue)
        <
            MAKI_AOS_TYPE(bool),
            machine<MachineConfHolder>&,
            detail::impl_of(conf).small_event_max_size,
            detail::impl_of(conf).small_event_max_align
        >;
    };

    struct empty_holder
    {
        template<bool = true> //Dummy template for lazy evaluation
        struct type{};
    };

    using rtc_queue_type = typename std::conditional_t
    <
        detail::impl_of(conf).run_to_completion,
        real_function_queue_holder,
        empty_holder
    >::template type<>;

    using event_deferral_queue_type = typename std::conditional_t
    <
        has_deferrable_events,
        real_function_queue_holder,
        empty_holder
    >::template type<>;

    template<detail::machine_operation Operation>
    struct any_event_visitor
    {
        template<class Event>
        static MAKI_AOS_TYPE(bool) call
        (
            const Event& event,
            machine<MachineConfHolder>& mach
        )
        {
            MAKI_AOS_RETURN MAKI_AOS_CALL impl_of(mach).template execute_one_operation<Operation>(mach, event);
        }
    };

    MAKI_AOS_TYPE(void) start_now(machine<MachineConfHolder>& mach)
    {
        MAKI_AOS_CALL execute_operation_now<detail::machine_operation::start>
        (
            mach,
            events::start{}
        );
    }

    template<class Event>
    MAKI_AOS_TYPE(void) start_no_catch
    (
        machine<MachineConfHolder>& mach,
        const Event& event
    )
    {
        if(!running())
        {
            MAKI_AOS_CALL execute_operation<detail::machine_operation::start>(mach, event);
        }
    }

    template<class Event>
    MAKI_AOS_TYPE(void) stop_no_catch
    (
        machine<MachineConfHolder>& mach,
        const Event& event
    )
    {
        if(running())
        {
            MAKI_AOS_CALL execute_operation<detail::machine_operation::stop>(mach, event);
        }
    }

    template<class Event>
    MAKI_AOS_TYPE(void) process_event_now_no_catch
    (
        machine<MachineConfHolder>& mach,
        const Event& event
    )
    {
        static_assert
        (
            detail::impl_of(conf).process_event_now_enabled,
            "`maki::machine_conf::process_event_now_enabled()` hasn't been set to `true`"
        );
        MAKI_AOS_CALL execute_operation_now<detail::machine_operation::process_event>(mach, event);
    }

    template<detail::machine_operation Operation, class Event>
    MAKI_AOS_TYPE(void) execute_operation
    (
        machine<MachineConfHolder>& mach,
        const Event& event
    )
    {
        if constexpr(detail::impl_of(conf).run_to_completion)
        {
            if(!executing_operation_) //If call is not recursive
            {
                MAKI_AOS_CALL execute_operation_now<Operation>(mach, event);
            }
            else
            {
                //Push event to RTC queue in case of recursive call
                push_event_impl<Operation>(event);
            }
        }
        else
        {
            MAKI_AOS_CALL execute_one_operation<Operation>(mach, event);
        }
    }

    template<detail::machine_operation Operation, class Event>
    MAKI_AOS_TYPE(void) execute_operation_now
    (
        machine<MachineConfHolder>& mach,
        const Event& event
    )
    {
        if constexpr(detail::impl_of(conf).run_to_completion)
        {
            auto grd = executing_operation_guard{*this};

            MAKI_AOS_CALL execute_one_operation<Operation>(mach, event);

            /*
            Process enqueued and deferred events, if any.
            We guarantee order of processing: At any given state configuration,
            if several pending events can be processed, they're processed in the
            same order they've been given to the `machine`.
            */
            MAKI_AOS_CALL try_processing_deferred_operations(mach);
            while (!rtc_queue_.empty())
            {
                MAKI_AOS_CALL rtc_queue_.invoke_and_pop(mach);
                MAKI_AOS_CALL try_processing_deferred_operations(mach);
            }
        }
        else
        {
            MAKI_AOS_CALL execute_one_operation<Operation>(mach, event);

            MAKI_AOS_CALL try_processing_deferred_operations(mach);
        }
    }

    template<class Event>
    MAKI_NOINLINE void push_event_no_catch(const Event& event)
    {
        static_assert(detail::impl_of(conf).run_to_completion);
        push_event_impl<detail::machine_operation::process_event>(event);
    }

    template<detail::machine_operation Operation, class Event>
    void push_event_impl(const Event& event)
    {
        rtc_queue_.template push<any_event_visitor<Operation>>(event);
    }

    /*
    Process all previously deferred events that can now be processed.
    */
    MAKI_AOS_TYPE(void) try_processing_deferred_operations
    (
        machine<MachineConfHolder>& mach
    )
    {
        if constexpr(has_deferrable_events)
        {
            /*
            The inner loop tries to process every deferred event once.

            The outer loop executes the inner loop as many times as necessary, that
            is, until `event_deferral_queue_` only contains events that are still
            deferred by any of the currently active states.

            These two levels are necessary, as processing a previously deferred
            event can change the active states and allow other events of
            `event_deferral_queue_` to be processed.
            */

            auto processing_count = 1;
            while (processing_count != 0) // Outer loop
            {
                processing_count = 0;
                for (auto i = 0U; i < event_deferral_queue_.size(); ++i) // Inner loop
                {
                    const auto processed = MAKI_AOS_CALL event_deferral_queue_.invoke_and_pop(mach);
                    processing_count += static_cast<int>(processed);
                }
            }
        }
    }

    template<detail::machine_operation Operation, class Event>
    MAKI_AOS_TYPE(bool) execute_one_operation
    (
        machine<MachineConfHolder>& mach,
        const Event& event
    )
    {
        if constexpr(Operation == detail::machine_operation::start)
        {
            MAKI_AOS_CALL impl_.template MAKI_AOS(enter)<MAKI_AOS_TYPE(void)>
            (
                mach,
                ctx_holder_.get(),
                event
            );
            MAKI_AOS_RETURN true;
        }
        else if constexpr(Operation == detail::machine_operation::stop)
        {
            MAKI_AOS_CALL impl_.template exit_to_finals<MAKI_AOS_TYPE(void)>
            (
                mach,
                context(),
                event
            );
            MAKI_AOS_RETURN true;
        }
        else
        {
            constexpr auto is_deferrable_event = detail::type_set_contains_v
            <
                deferrable_event_type_set,
                Event
            >;

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

            // Defer the event if required by any of the active states
            if constexpr(is_deferrable_event)
            {
                if(impl_.template defers_event<Event>())
                {
                    event_deferral_queue_.template push<any_event_visitor<Operation>>(event);
                    MAKI_AOS_RETURN false;
                }
            }

            //If running, execute pre-processing hook for `Event`, if any.
            if constexpr(has_matching_pre_processing_hook)
            {
                if(running())
                {
                    detail::call_matching_event_action<void, pre_processing_hook_ptr_constant_list>
                    (
                        mach,
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
                    const auto processed = MAKI_AOS_CALL impl_.template MAKI_AOS(call_internal_action)<aos_type, false>(mach, context(), event);

                    detail::call_matching_event_action<void, post_processing_hook_ptr_constant_list>
                    (
                        mach,
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

                MAKI_AOS_CALL impl_.template MAKI_AOS(call_internal_action)<aos_type, false>(mach, context(), event);
            }

            MAKI_AOS_RETURN true;
        }
    }

    static constexpr auto pre_processing_hooks = detail::impl_of(conf).pre_processing_hooks;
    static constexpr auto post_processing_hooks = detail::impl_of(conf).post_processing_hooks;

    using pre_processing_hook_ptr_constant_list = detail::mix_constant_list_t<pre_processing_hooks>;
    using post_processing_hook_ptr_constant_list = detail::mix_constant_list_t<post_processing_hooks>;

    detail::context_holder
    <
        context_type,
        detail::context_storage::plain,
        detail::impl_of(conf).context_sig
    > ctx_holder_;

    impl_type impl_;

    bool executing_operation_ = false;

    /*
    Storage for operations that have been postponed by the run-to-completion
    mechanism.
    */
    rtc_queue_type rtc_queue_;

    /*
    Storage for operations that have been postponed by the event deferral
    mechanism.
    */
    event_deferral_queue_type event_deferral_queue_;
};

#undef MAKI_DETAIL_MAYBE_CATCH

} //namespace
