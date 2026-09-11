//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

namespace maki
{
    template<class Impl>
    class region;
}

namespace maki::detail
{

namespace MAKI_AOS_NAME(region_detail)
{
    inline constexpr auto null_action_index = -1;
    inline constexpr auto null_guard_index = -1;

    namespace filter_state_mold_iseq_by_state_set_detail
    {
        template<class MachineConfHolder, class TransitionTablePath, int TransitionIndex>
        struct for_state_set
        {
            static constexpr const auto& trans_table =
                machine_conf_tree::node_at_path_v<MachineConfHolder, TransitionTablePath>
            ;

            static constexpr const auto& trans =
                tuple_get<TransitionIndex>(impl_of(trans_table))
            ;

            template<int StateMoldId>
            struct matches
            {
                static constexpr const auto& target_state_mold =
                    machine_conf_tree::node_at_path_v
                    <
                        MachineConfHolder,
                        iseq_push_back_t
                        <
                            TransitionTablePath,
                            StateMoldId
                        >
                    >
                ;

                static constexpr bool value = contains(impl_of(trans.source_state_mold), target_state_mold);
            };
        };
    }

    template<class MachineConfHolder, class TransitionTablePath, int TransitionIndex, class StateMoldIseq>
    using filter_state_mold_iseq_by_state_set_t = iseq_filter_t
    <
        StateMoldIseq,
        filter_state_mold_iseq_by_state_set_detail::for_state_set<MachineConfHolder, TransitionTablePath, TransitionIndex>::template matches
    >;
}

template<class MachineConfHolder, class TransitionTablePath, context_storage ParentCtxStorage>
class MAKI_AOS_NAME(region_impl)
{
public:
    static constexpr const auto& machine_conf = MachineConfHolder::value;

    using machine_conf_type = std::decay_t<decltype(detail::impl_of(machine_conf))>;

    static constexpr auto trans_table = machine_conf_tree::node_at_path_v<MachineConfHolder, TransitionTablePath>;

    using transition_table_type = std::decay_t<decltype(trans_table)>;

    using transition_table_digest_type =
        transition_table_digest<MachineConfHolder, TransitionTablePath>
    ;

    using state_mold_iseq_0 =
        typename transition_table_digest_type::stt_mold_ids
    ;

    using state_mold_iseq =
        iseq_push_back_t
        <
            state_mold_iseq_0,
            state_mold_ids::undefined
        >
    ;

    template<int... StateMoldIds>
    using state_mold_iseq_to_state_mix_t = mix
    <
        maki::state
        <
            MAKI_AOS_NAME(state_impl_t)
            <
                MachineConfHolder,
                iseq_push_back_t<TransitionTablePath, StateMoldIds>,
                ParentCtxStorage
            >
        >...
    >;

    using state_mix_type = iseq_apply_t
    <
        state_mold_iseq_0,
        state_mold_iseq_to_state_mix_t
    >;

    using states_event_type_set = state_type_list_event_type_set_t<state_mix_type>;

    using event_type_set = type_set_union_t
    <
        transition_table_event_type_set_t<transition_table_type>,
        states_event_type_set
    >;

    using deferrable_event_type_set = state_type_list_deferrable_event_type_set_t<state_mix_type>;

#if MAKI_AOS_ASYNC
    template<class T>
    using awaitable_type = detail::co_util::awaitable_t<typename machine_conf_type::awaitable_template_holder, T>;
#endif

    template<class Context>
    MAKI_AOS_NAME(region_impl)(const region<MAKI_AOS_NAME(region_impl)>* pitf, MAKI_AOS_NAME(machine)<MachineConfHolder>& mach, Context& ctx):
        pitf_(pitf),
        states_(mix_uniform_construct, mach, ctx)
    {
    }

    MAKI_AOS_NAME(region_impl)(const MAKI_AOS_NAME(region_impl)&) = delete;
    MAKI_AOS_NAME(region_impl)(MAKI_AOS_NAME(region_impl)&&) = delete;
    MAKI_AOS_NAME(region_impl)& operator=(const MAKI_AOS_NAME(region_impl)&) = delete;
    MAKI_AOS_NAME(region_impl)& operator=(MAKI_AOS_NAME(region_impl)&&) = delete;
    ~MAKI_AOS_NAME(region_impl)() = default;

    template<const auto& StateMold>
    [[nodiscard]] bool is() const
    {
        if constexpr(is_state_set_v<std::decay_t<decltype(StateMold)>>)
        {
            return is_active_state_mold_in_set<&StateMold>();
        }
        else
        {
            return is_active_state_mold<StateMold>();
        }
    }

    [[nodiscard]] bool completed() const
    {
        return active_state_mold_id_ == state_mold_ids::fin;
    }

    template<class Event>
    [[nodiscard]] bool defers_event() const
    {
        if constexpr(type_set_contains_v<deferrable_event_type_set, Event>)
        {
            auto defers = false;
            with_active_state_mold
            <
                state_mold_iseq,
                state_defers_event<Event>
            >(*this, defers);
            return defers;
        }
        else
        {
            return false;
        }
    }

    template<class Context>
    void emplace_contexts_with_parent_lifetime(Context& ctx, machine<MachineConfHolder>& mach)
    {
        iseq_for_each
        <
            state_mold_iseq_0,
            state_emplace_contexts_with_parent_lifetime
        >(*this, ctx, mach);
    }

    // Enter the initial state
    template<class Context, class Event>
    MAKI_AOS_TYPE(void) enter(MAKI_AOS_NAME(machine)<MachineConfHolder>& mach, Context& ctx, const Event& event)
    {
        MAKI_AOS_CALL execute_transition
        <
            state_mold_ids::null,
            state_mold_ids::ini,
            0
        >(mach, ctx, event);
    }

    // Exit the active state
    template<int TargetStateMoldId, class Context, class Event>
    MAKI_AOS_TYPE(void) exit(MAKI_AOS_NAME(machine)<MachineConfHolder>& mach, Context& ctx, const Event& event)
    {
        if(!completed())
        {
            MAKI_AOS_CALL MAKI_AOS_NAME(with_active_state_mold)<state_mold_iseq, exit_2<TargetStateMoldId>>
            (
                *this,
                mach,
                ctx,
                event
            );
        }
    }

    void reset_contexts_with_parent_lifetime()
    {
        iseq_for_each
        <
            state_mold_iseq_0,
            state_reset_contexts_with_parent_lifetime
        >(*this);
    }

    template<bool Dry, class Context, class Event>
    MAKI_AOS_TYPE(bool) process_event(MAKI_AOS_NAME(machine)<MachineConfHolder>& mach, Context& ctx, const Event& event)
    {
        MAKI_AOS_RETURN MAKI_AOS_CALL process_event_2<Dry>(*this, mach, ctx, event);
    }

    template<bool Dry, class Context, class Event>
    MAKI_AOS_TYPE(bool) process_event(const MAKI_AOS_NAME(machine)<MachineConfHolder>& mach, Context& ctx, const Event& event) const
    {
        MAKI_AOS_RETURN MAKI_AOS_CALL process_event_2<Dry>(*this, mach, ctx, event);
    }

    template<const auto& StateMold>
    const auto& state() const
    {
        constexpr int state_mold_id = machine_conf_tree::id_of_state_mold_v
        <
            MachineConfHolder,
            TransitionTablePath,
            StateMold
        >;
        return state_mold_id_to_state<state_mold_id>();
    }

    static const auto& path()
    {
        static const auto value = maki::path{path_impl<MachineConfHolder, TransitionTablePath>{}};
        return value;
    }

private:
    struct state_emplace_contexts_with_parent_lifetime
    {
        template<int StateMoldId, class Self, class Context>
        static void call
        (
            Self& self,
            Context& ctx,
            machine<MachineConfHolder>& mach
        )
        {
            auto& stt = static_state_mold_id_to_state<StateMoldId>(self);
            impl_of(stt).emplace_contexts_with_parent_lifetime(ctx, mach);
        }
    };

    struct state_reset_contexts_with_parent_lifetime
    {
        template<int StateMoldId, class Self>
        static void call(Self& self)
        {
            auto& stt = static_state_mold_id_to_state<StateMoldId>(self);
            impl_of(stt).reset_contexts_with_parent_lifetime();
        }
    };

    template<class Event>
    struct state_defers_event
    {
        template<int StateMoldId>
        static void call(const MAKI_AOS_NAME(region_impl)& self, bool& defers)
        {
            const auto& stt = self.state_mold_id_to_state<StateMoldId>();
            defers = impl_of(stt).template defers_event<Event>();
        }
    };

    template<bool Dry, class Self, class Machine, class Context, class Event>
    static MAKI_AOS_TYPE(bool) process_event_2
    (
        Self& self,
        Machine& mach,
        Context& ctx,
        const Event& event
    )
    {
        //List the transitions whose event set contains `Event`
        using candidate_transition_iseq = transition_table_filters::by_event_t
        <
            MachineConfHolder,
            TransitionTablePath,
            Event
        >;

        constexpr auto must_try_executing_transitions = !iseq_empty_v<candidate_transition_iseq>;

        constexpr auto must_try_process_event_in_states = type_set_contains_v
        <
            states_event_type_set,
            Event
        >;

        if constexpr(must_try_executing_transitions && must_try_process_event_in_states)
        {
            /*
            There is a possibility of conflicting transition in this case.
            Note that nested transitions take precedence over higher-order
            transitions.
            */
            MAKI_AOS_RETURN
                call_active_state_internal_action<Dry>(self, mach, ctx, event) ||
                try_executing_transitions<candidate_transition_iseq, Dry>(self, mach, ctx, event)
            ;
        }
        else if constexpr(!must_try_executing_transitions && must_try_process_event_in_states)
        {
            MAKI_AOS_RETURN call_active_state_internal_action<Dry>(self, mach, ctx, event);
        }
        else if constexpr(must_try_executing_transitions && !must_try_process_event_in_states)
        {
            MAKI_AOS_RETURN MAKI_AOS_CALL try_executing_transitions<candidate_transition_iseq, Dry>(self, mach, ctx, event);
        }
        else
        {
            MAKI_AOS_RETURN false;
        }
    }

    template<int TargetStateMoldId>
    struct exit_2
    {
        template<int ActiveStateMoldId, class Context, class Event>
        static MAKI_AOS_TYPE(void) call(MAKI_AOS_NAME(region_impl)& self, machine<MachineConfHolder>& mach, Context& ctx, const Event& event)
        {
            MAKI_AOS_CALL self.execute_transition
            <
                ActiveStateMoldId,
                TargetStateMoldId,
                MAKI_AOS_NAME(region_detail)::null_action_index
            >(mach, ctx, event);
        }
    };

    /*
    Try executing one of the transitions at indices
    `TransitionIndexConstantList`.
    */
    template<class TransitionIseq, bool Dry = false, class Self, class Machine, class Context, class Event>
    static MAKI_AOS_TYPE(bool) try_executing_transitions(Self& self, Machine& mach, Context& ctx, const Event& event)
    {
        MAKI_AOS_RETURN MAKI_AOS_CALL MAKI_AOS_NAME(iseq_for_each_or)
        <
            MAKI_AOS_TYPE(bool),
            TransitionIseq,
            try_executing_transition<Dry>
        >(self, mach, ctx, event);
    }

    // Try executing the transition at index `TransitionIndex`.
    template<bool Dry>
    struct try_executing_transition
    {
        template<int TransitionIndex, class Self, class Machine, class Context, class Event, class... ExtraArgs>
        static MAKI_AOS_TYPE(bool) call(Self& self, Machine& mach, Context& ctx, const Event& event, ExtraArgs&... extra_args)
        {
            static constexpr const auto& trans = tuple_get<TransitionIndex>(impl_of(trans_table));

            static constexpr auto target_state_mold_id =
                machine_conf_tree::id_of_target_state_mold_v
                <
                    MachineConfHolder,
                    TransitionTablePath,
                    TransitionIndex
                >
            ;

            if constexpr(is_state_set_v<std::decay_t<decltype(trans.source_state_mold)>>)
            {
                //List of state molds that belong to the source state set
                using matching_state_mold_iseq = MAKI_AOS_NAME(region_detail)::filter_state_mold_iseq_by_state_set_t
                <
                    MachineConfHolder,
                    TransitionTablePath,
                    TransitionIndex,
                    state_mold_iseq
                >;

                static_assert(iseq_size_v<matching_state_mold_iseq> != 0);

                MAKI_AOS_RETURN MAKI_AOS_CALL MAKI_AOS_NAME(iseq_for_each_or)
                <
                    MAKI_AOS_TYPE(bool),
                    matching_state_mold_iseq,
                    try_executing_transition_2
                    <
                        Dry,
                        target_state_mold_id,
                        TransitionIndex,
                        TransitionIndex
                    >
                >(self, mach, ctx, event, extra_args...);
            }
            else
            {
                static constexpr auto source_state_mold_id =
                    machine_conf_tree::id_of_source_state_mold_v
                    <
                        MachineConfHolder,
                        TransitionTablePath,
                        TransitionIndex
                    >
                ;

                MAKI_AOS_RETURN MAKI_AOS_CALL try_executing_transition_2
                <
                    Dry,
                    target_state_mold_id,
                    TransitionIndex,
                    TransitionIndex
                >::template call<source_state_mold_id>
                (
                    self,
                    mach,
                    ctx,
                    event
                );
            }
        }
    };

    template<bool Dry, int TargetStateMoldId, int ActionIndex, int GuardIndex>
    struct try_executing_transition_2
    {
        template
        <
            int SourceStateMoldId,
            class Self,
            class Machine,
            class Context,
            class Event
        >
        static MAKI_AOS_TYPE(bool) call
        (
            Self& self,
            Machine& mach,
            Context& ctx,
            const Event& event
        )
        {
            if constexpr(!is_null_v<Event>) // Already filtered out
            {
                //Make sure the transition source state is the active state
                if(self.active_state_mold_id_ != SourceStateMoldId)
                {
                    MAKI_AOS_RETURN false;
                }
            }

            //Check guard
            if constexpr(GuardIndex != MAKI_AOS_NAME(region_detail)::null_guard_index)
            {
                const auto& guard = tuple_get<GuardIndex>(impl_of(trans_table)).grd;
                if(!detail::call_guard(guard, ctx, mach, event))
                {
                    MAKI_AOS_RETURN false;
                }
            }

            if constexpr(!Dry)
            {
                MAKI_AOS_CALL self.template execute_transition
                <
                    SourceStateMoldId,
                    TargetStateMoldId,
                    ActionIndex
                >(mach, ctx, event);
            }

            MAKI_AOS_RETURN true;
        }
    };

    template
    <
        int SourceStateMoldId,
        int TargetStateMoldId,
        int ActionIndex,
        class Context,
        class Event
    >
    MAKI_AOS_TYPE(void) execute_transition
    (
        MAKI_AOS_NAME(machine)<MachineConfHolder>& mach,
        Context& ctx,
        const Event& event
    )
    {
        /*
        For external transitions, invoke the pre-transition hook, if any.
        */
        if constexpr
        (
            TargetStateMoldId != state_mold_ids::internal &&
            !equals(impl_of(MachineConfHolder::value).pre_external_transition_hook, null)
        )
        {
            impl_of(MachineConfHolder::value).pre_external_transition_hook
            (
                ctx,
                *pitf_,
                state_mold_id_to_state<SourceStateMoldId>(),
                state_mold_id_to_state<TargetStateMoldId>(),
                event
            );
        }

        /*
        For external transitions, change the active state to `undefined` (useful
        in case of exception).
        */
        if constexpr
        (
            TargetStateMoldId != state_mold_ids::internal &&
            TargetStateMoldId != state_mold_ids::null
        )
        {
            active_state_mold_id_ = state_mold_ids::undefined;
        }

        /*
        For external transitions, invoke the exit action of the source state, if
        any.
        */
        if constexpr(TargetStateMoldId != state_mold_ids::internal)
        {
            MAKI_AOS_CALL impl_of(state_mold_id_to_state<SourceStateMoldId>()).template exit<MAKI_AOS_TYPE(void)>
            (
                mach,
                ctx,
                event
            );
        }

        /*
        Invoke the transition action, if any.
        */
        if constexpr(ActionIndex != MAKI_AOS_NAME(region_detail)::null_action_index)
        {
            MAKI_AOS_CALL detail::MAKI_AOS_NAME(call_action)<MAKI_AOS_TYPE(void)>
            (
                tuple_get<ActionIndex>(impl_of(trans_table)).act,
                ctx,
                mach,
                event
            );
        }

        /*
        For external transitions, invoke the entry action of the target state,
        if any.
        */
        if constexpr(TargetStateMoldId != state_mold_ids::internal)
        {
            auto& target_state = state_mold_id_to_state<TargetStateMoldId>();

            MAKI_AOS_CALL impl_of(target_state).template enter<MAKI_AOS_TYPE(void)>
            (
                mach,
                ctx,
                event
            );
        }

        /*
        For external transitions, change the active state to the target state.
        */
        if constexpr
        (
            TargetStateMoldId != state_mold_ids::internal &&
            TargetStateMoldId != state_mold_ids::null
        )
        {
            active_state_mold_id_ = TargetStateMoldId;
        }

        /*
        For external transitions, invoke the post-transition hook, if any.
        */
        if constexpr
        (
            TargetStateMoldId != state_mold_ids::internal &&
            !equals(impl_of(MachineConfHolder::value).post_external_transition_hook, null)
        )
        {
            impl_of(MachineConfHolder::value).post_external_transition_hook
            (
                ctx,
                *pitf_,
                state_mold_id_to_state<SourceStateMoldId>(),
                state_mold_id_to_state<TargetStateMoldId>(),
                event
            );
        }

        /*
        For external transitions, execute the completion transitions, if any.
        */
        if constexpr
        (
            TargetStateMoldId != state_mold_ids::internal &&
            TargetStateMoldId != state_mold_ids::null &&
            transition_table_digest_type::has_completion_transitions
        )
        {
            MAKI_AOS_CALL try_executing_completion_transitions
            (
                state_mold_id_to_state<TargetStateMoldId>(),
                mach,
                ctx
            );
        }
    }

    // Find the active state and call its internal action for `event`.
    template<bool Dry, class Self, class Machine, class Context, class Event>
    static MAKI_AOS_TYPE(bool) call_active_state_internal_action
    (
        Self& self,
        Machine& mach,
        Context& ctx,
        const Event& event
    )
    {
        auto processed = false;
        MAKI_AOS_CALL MAKI_AOS_NAME(iseq_for_each_or)
        <
            MAKI_AOS_TYPE(bool),
            state_mold_iseq_0,
            call_active_state_internal_action_2<Dry>
        >(self, mach, ctx, event, processed);
        MAKI_AOS_RETURN processed;
    }

    template<bool Dry>
    struct call_active_state_internal_action_2
    {
        template<int StateMoldId, class Self, class Machine, class Context, class Event>
        static MAKI_AOS_TYPE(bool) call
        (
            Self& self,
            Machine& mach,
            Context& ctx,
            const Event& event,
            bool& processed
        )
        {
            auto& state = static_state_mold_id_to_state<StateMoldId>(self);
            using state_type = std::decay_t<decltype(state)>;

            constexpr auto can_state_process_event =
                type_set_contains_v
                <
                    typename impl_of_t<state_type>::event_type_set,
                    Event
                >
            ;

            if constexpr(can_state_process_event)
            {
                if(StateMoldId != self.active_state_mold_id_)
                {
                    MAKI_AOS_RETURN false;
                }

                processed = MAKI_AOS_CALL impl_of(state).template call_internal_action<Dry>
                (
                    mach,
                    ctx,
                    event
                );

                if constexpr
                (
                    transition_table_digest_type::has_completion_transitions &&
                    !Dry
                )
                {
                    self.try_executing_completion_transitions
                    (
                        state,
                        mach,
                        ctx
                    );
                }

                MAKI_AOS_RETURN true;
            }
            else
            {
                MAKI_AOS_RETURN false;
            }
        }
    };

    template<class ActiveState, class Context>
    MAKI_AOS_TYPE(void) try_executing_completion_transitions
    (
        ActiveState& active_state,
        machine<MachineConfHolder>& mach,
        Context& ctx
    )
    {
        static constexpr const auto& active_state_mold = impl_of_t<ActiveState>::mold;

        using candidate_transition_iseq = transition_table_filters::by_source_state_and_null_event_t
        <
            MachineConfHolder,
            TransitionTablePath,
            active_state_mold
        >;

        if constexpr(!iseq_empty_v<candidate_transition_iseq>)
        {
            if(impl_of(active_state).completed())
            {
                try_executing_transitions<candidate_transition_iseq>(*this, mach, ctx, null);
            }
        }
    }

    template<const auto& StateMold>
    [[nodiscard]] bool is_active_state_mold() const
    {
        return active_state_mold_id_ == machine_conf_tree::id_of_state_mold_v<MachineConfHolder, TransitionTablePath, StateMold>;
    }

    template<auto StateSetPtr>
    [[nodiscard]] bool is_active_state_mold_in_set() const
    {
        auto matches = false;
        with_active_state_mold
        <
            iseq_push_back_t
            <
                state_mold_iseq,
                state_mold_ids::fin
            >,
            is_active_state_mold_in_set_2<StateSetPtr>
        >(matches);
        return matches;
    }

    template<auto StateSetPtr>
    struct is_active_state_mold_in_set_2
    {
        template<int ActiveStateMoldId>
        static void call([[maybe_unused]] bool& matches)
        {
            constexpr const auto& active_state_mold =
                machine_conf_tree::node_at_path_v
                <
                    MachineConfHolder,
                    iseq_push_back_t
                    <
                        TransitionTablePath,
                        ActiveStateMoldId
                    >
                >
            ;

            if constexpr(contains(impl_of(*StateSetPtr), active_state_mold))
            {
                matches = true;
            }
        }
    };

    template<class StateMoldIseq, class F, class... Args>
    void with_active_state_mold(Args&&... args) const
    {
        iseq_for_each_or
        <
            bool,
            StateMoldIseq,
            with_active_state_mold_2<F>
        >(*this, std::forward<Args>(args)...);
    }

    template<class F>
    struct with_active_state_mold_2
    {
        template<int StateMoldId, class... Args>
        static bool call(const MAKI_AOS_NAME(region_impl)& self, Args&&... args)
        {
            if(self.active_state_mold_id_ == StateMoldId)
            {
                F::template call<StateMoldId>(std::forward<Args>(args)...);
                return true;
            }
            return false;
        }
    };

#if MAKI_AOS_ASYNC
    template<class StateMoldIseq, class F, class... Args>
    MAKI_AOS_TYPE(void) async_with_active_state_mold(Args&&... args) const
    {
        co_await MAKI_AOS_NAME(iseq_for_each_or)
        <
            MAKI_AOS_TYPE(bool),
            StateMoldIseq,
            async_with_active_state_mold_2<F>
        >(*this, std::forward<Args>(args)...);
    }

    template<class F>
    struct async_with_active_state_mold_2
    {
        template<int StateMoldId, class... Args>
        static MAKI_AOS_TYPE(bool) call(const MAKI_AOS_NAME(region_impl)& self, Args&&... args)
        {
            if(self.active_state_mold_id_ == StateMoldId)
            {
                F::template call<StateMoldId>(std::forward<Args>(args)...);
                co_return true;
            }
            co_return false;
        }
    };
#endif

    template<int StateMoldId>
    auto& state_mold_id_to_state()
    {
        return static_state_mold_id_to_state<StateMoldId>(*this);
    }

    template<int StateMoldId>
    const auto& state_mold_id_to_state() const
    {
        return static_state_mold_id_to_state<StateMoldId>(*this);
    }

    //Note: We use static to factorize const and non-const Region
    template<int StateMoldId, class Region>
    static auto& static_state_mold_id_to_state(Region& self)
    {
        if constexpr(StateMoldId == state_mold_ids::undefined)
        {
            return states::MAKI_AOS_NAME(undefined);
        }
        else if constexpr(StateMoldId == state_mold_ids::null)
        {
            return states::MAKI_AOS_NAME(null);
        }
        else if constexpr(StateMoldId == state_mold_ids::fin)
        {
            return states::MAKI_AOS_NAME(fin);
        }
        else
        {
            using state_t =
                maki::state
                <
                    MAKI_AOS_NAME(state_impl_t)
                    <
                        MachineConfHolder,
                        iseq_push_back_t<TransitionTablePath, StateMoldId>,
                        ParentCtxStorage
                    >
                >
            ;
            return get<state_t>(self.states_);
        }
    }

    const region<MAKI_AOS_NAME(region_impl)>* pitf_;
    state_mix_type states_;
    int active_state_mold_id_ = state_mold_ids::fin;
};

} //namespace
