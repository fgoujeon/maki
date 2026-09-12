//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

namespace maki::detail::state_impls
{

template
<
    class MachineConfHolder,
    class ParentStateMoldPath,
    context_storage ParentCtxStorage,
    int Index
>
struct MAKI_AOS_NAME(region_mix_elem)
{
    using transition_table_path = iseq_push_back_t<ParentStateMoldPath, Index>;
    using type = region<MAKI_AOS_NAME(region_impl)<MachineConfHolder, transition_table_path, ParentCtxStorage>>;
};

template
<
    class MachineConfHolder,
    class ParentStateMoldPath,
    context_storage ParentCtxStorage,
    int Index
>
using MAKI_AOS_NAME(region_mix_elem_t) = typename MAKI_AOS_NAME(region_mix_elem)<MachineConfHolder, ParentStateMoldPath, ParentCtxStorage, Index>::type;

template
<
    class MachineConfHolder,
    class ParentStateMoldPath,
    context_storage ParentCtxStorage,
    class RegionIndexSequence
>
struct MAKI_AOS_NAME(region_mix);

template
<
    class MachineConfHolder,
    class ParentStateMoldPath,
    context_storage ParentCtxStorage,
    int... RegionIndexes
>
struct MAKI_AOS_NAME(region_mix)
<
    MachineConfHolder,
    ParentStateMoldPath,
    ParentCtxStorage,
    iseq<RegionIndexes...>
>
{
    using type = mix
    <
        MAKI_AOS_NAME(region_mix_elem_t)
        <
            MachineConfHolder,
            ParentStateMoldPath,
            ParentCtxStorage,
            RegionIndexes
        >...
    >;
};

template<class EventTypeSet, class Region>
using MAKI_AOS_NAME(region_type_list_event_type_set_operation) =
    type_set_union_t
    <
        EventTypeSet,
        typename impl_of_t<Region>::event_type_set
    >
;

template<class RegionTypeList>
using MAKI_AOS_NAME(region_type_list_event_type_set) = tlu::left_fold_t
<
    RegionTypeList,
    MAKI_AOS_NAME(region_type_list_event_type_set_operation),
    empty_type_set_t
>;

template<class EventTypeSet, class Region>
using MAKI_AOS_NAME(region_type_list_deferrable_event_type_set_operation) =
    type_set_union_t
    <
        EventTypeSet,
        typename impl_of_t<Region>::deferrable_event_type_set
    >
;

template<class RegionTypeList>
using MAKI_AOS_NAME(region_type_list_deferrable_event_type_set) = tlu::left_fold_t
<
    RegionTypeList,
    MAKI_AOS_NAME(region_type_list_deferrable_event_type_set_operation),
    empty_type_set_t
>;

template<class MachineConfHolder, class StateMoldPath, context_storage ParentCtxStorage>
class MAKI_AOS_NAME(composite_no_context)
{
public:
    using machine_conf_holder_type = MachineConfHolder;
    using state_mold_path = StateMoldPath;

    static constexpr const auto& mold = machine_conf_tree::node_at_path_v<MachineConfHolder, StateMoldPath>;
    using mold_type = std::decay_t<decltype(mold)>;
    using option_set_type = std::decay_t<decltype(impl_of(mold))>;
    using transition_table_type_list = decltype(impl_of(mold).transition_tables);
    using impl_type = simple_no_context<MachineConfHolder, StateMoldPath>;

    static constexpr auto ctx_lifetime = impl_of(mold).context_lifetime;

    static constexpr auto ctx_storage =
        ctx_lifetime == state_context_lifetime::parent ?
        ParentCtxStorage :
        context_storage::optional
    ;

    using region_index_sequence = linear_iseq_t
    <
        impl_of(mold).transition_tables.size
    >;

    using region_mix_type = typename MAKI_AOS_NAME(region_mix)
    <
        MachineConfHolder,
        StateMoldPath,
        ctx_storage,
        region_index_sequence
    >::type;

    using event_type_set = type_set_union_t
    <
        typename impl_type::event_type_set,
        MAKI_AOS_NAME(region_type_list_event_type_set)<region_mix_type>
    >;

    using deferrable_event_type_set = type_set_union_t
    <
        typename impl_type::deferrable_event_type_set,
        MAKI_AOS_NAME(region_type_list_deferrable_event_type_set)<region_mix_type>
    >;

    template<class Context>
    MAKI_AOS_NAME(composite_no_context)(MAKI_AOS_NAME(machine)<MachineConfHolder>& mach, Context& ctx):
        regions_(mix_uniform_construct, mach, ctx)
    {
    }

    MAKI_AOS_NAME(composite_no_context)(const MAKI_AOS_NAME(composite_no_context)&) = delete;
    MAKI_AOS_NAME(composite_no_context)(MAKI_AOS_NAME(composite_no_context)&&) = delete;
    MAKI_AOS_NAME(composite_no_context)& operator=(const MAKI_AOS_NAME(composite_no_context)&) = delete;
    MAKI_AOS_NAME(composite_no_context)& operator=(MAKI_AOS_NAME(composite_no_context)&&) = delete;
    ~MAKI_AOS_NAME(composite_no_context)() = default;

    template<class Context>
    void emplace_contexts_with_parent_lifetime(Context& ctx, machine<MachineConfHolder>& mach)
    {
        tlu::for_each
        <
            void,
            region_mix_type,
            region_emplace_contexts_with_parent_lifetime
        >(*this, ctx, mach);
    }

    template<class AosVoid, class Context, class Event>
    AosVoid MAKI_AOS_NAME(enter)
    (
        MAKI_AOS_NAME(machine)<MachineConfHolder>& mach,
        Context& ctx,
        const Event& event
    )
    {
        MAKI_AOS_CALL impl_type::template MAKI_AOS_NAME(enter)<AosVoid>(mach, ctx, event);
        MAKI_AOS_CALL tlu::MAKI_AOS_NAME(for_each)
        <
            AosVoid,
            region_mix_type,
            region_enter<AosVoid>
        >(*this, mach, ctx, event);
    }

    template<template<class> class AosType, bool Dry, class Context, class Event>
    AosType<bool> MAKI_AOS_NAME(call_internal_action)
    (
        MAKI_AOS_NAME(machine)<MachineConfHolder>& mach,
        Context& ctx,
        const Event& event
    )
    {
        MAKI_AOS_RETURN MAKI_AOS_CALL call_internal_action_2<AosType, Dry>(*this, mach, ctx, event);
    }

    template<template<class> class AosType, bool Dry, class Context, class Event>
    AosType<bool> MAKI_AOS_NAME(call_internal_action)
    (
        const MAKI_AOS_NAME(machine)<MachineConfHolder>& mach,
        Context& ctx,
        const Event& event
    ) const
    {
        MAKI_AOS_RETURN MAKI_AOS_CALL call_internal_action_2<AosType, Dry>(*this, mach, ctx, event);
    }

    template<class AosVoid, class Context, class Event>
    AosVoid MAKI_AOS_NAME(exit)
    (
        MAKI_AOS_NAME(machine)<MachineConfHolder>& mach,
        Context& ctx,
        const Event& event
    )
    {
        MAKI_AOS_CALL tlu::MAKI_AOS_NAME(for_each)
        <
            AosVoid,
            region_mix_type,
            region_exit<AosVoid, state_mold_ids::null>
        >(*this, mach, ctx, event);

        MAKI_AOS_CALL impl_type::template MAKI_AOS_NAME(exit)<AosVoid>(mach, ctx, event);
    }

    // For each region, transition from active state to final state.
    template<class AosVoid, class Context, class Event>
    AosVoid exit_to_finals(machine<MachineConfHolder>& mach, Context& ctx, const Event& event)
    {
        MAKI_AOS_CALL tlu::MAKI_AOS_NAME(for_each)
        <
            AosVoid,
            region_mix_type,
            region_exit<AosVoid, state_mold_ids::fin>
        >(*this, mach, ctx, event);

        MAKI_AOS_CALL impl_type::template MAKI_AOS_NAME(exit)<AosVoid>(mach, ctx, event);
    }

    void reset_contexts_with_parent_lifetime()
    {
        tlu::for_each
        <
            void,
            region_mix_type,
            region_reset_contexts_with_parent_lifetime
        >(*this);
    }

    template<class Event>
    [[nodiscard]] bool defers_event() const
    {
        if constexpr(type_set_contains_v<deferrable_event_type_set, Event>)
        {
            return
                impl_type::template defers_event<Event>() ||
                tlu::for_each_or
                <
                    region_mix_type,
                    region_defers_event<Event>
                >(*this)
            ;
        }
        else
        {
            return false;
        }
    }

    template<int Index>
    [[nodiscard]] const auto& region() const
    {
        using region_type = tlu::get_t<region_mix_type, Index>;
        return get<region_type>(regions_);
    }

    template<const auto& StateMold>
    [[nodiscard]] const auto& state() const
    {
        static_assert(region_mix_type::size == 1);
        return impl_of(region<0>()).template state<StateMold>();
    }

    template<const auto& StateMold>
    [[nodiscard]] bool is() const
    {
        static_assert(region_mix_type::size == 1);
        return impl_of(region<0>()).template is<StateMold>();
    }

    [[nodiscard]] bool completed() const
    {
        return tlu::apply_t
        <
            region_mix_type,
            all_regions_completed
        >::call(*this);
    }

private:
    template<class... Regions>
    struct all_regions_completed
    {
        template<class Self>
        static bool call(const Self& self)
        {
            return (impl_of(get<Regions>(self.regions_)).completed() && ...);
        }
    };

    struct region_emplace_contexts_with_parent_lifetime
    {
        template<class Region, class Self, class Context>
        static void call(Self& self, Context& ctx, machine<MachineConfHolder>& mach)
        {
            impl_of(get<Region>(self.regions_)).emplace_contexts_with_parent_lifetime(ctx, mach);
        }
    };

    template<class AosVoid>
    struct region_enter
    {
        template<class Region, class Self, class Context, class Event>
        static AosVoid call(Self& self, MAKI_AOS_NAME(machine)<MachineConfHolder>& mach, Context& ctx, const Event& event)
        {
            MAKI_AOS_CALL impl_of(get<Region>(self.regions_)).enter(mach, ctx, event);
        }
    };

    template<class AosInt, bool Dry>
    struct region_process_event
    {
        template<class Region, class Self, class MachineArg, class Context, class Event>
        static AosInt call(Self& self, MachineArg& mach, Context& ctx, const Event& event)
        {
            const auto processed = MAKI_AOS_CALL impl_of(get<Region>(self.regions_)).template process_event<Dry>(mach, ctx, event);
            MAKI_AOS_RETURN static_cast<int>(processed);
        }
    };

    template<class AosVoid, int TargetStateMoldId>
    struct region_exit
    {
        template<class Region, class Self, class Context, class Event>
        static AosVoid call(Self& self, machine<MachineConfHolder>& mach, Context& ctx, const Event& event)
        {
            MAKI_AOS_CALL impl_of(get<Region>(self.regions_)).template exit<TargetStateMoldId>(mach, ctx, event);
        }
    };

    struct region_reset_contexts_with_parent_lifetime
    {
        template<class Region, class Self>
        static void call(Self& self)
        {
            impl_of(get<Region>(self.regions_)).reset_contexts_with_parent_lifetime();
        }
    };

    template<class Event>
    struct region_defers_event
    {
        template<class Region>
        static bool call(const MAKI_AOS_NAME(composite_no_context)& self)
        {
            return impl_of(get<Region>(self.regions_)).template defers_event<Event>();
        }
    };

    template<template<class> class AosType, bool Dry, class Self, class MachineArg, class Context, class Event>
    static AosType<bool> call_internal_action_2
    (
        Self& self,
        MachineArg& mach,
        Context& ctx,
        const Event& event
    )
    {
        constexpr auto can_process_event = type_set_contains_v
        <
            typename impl_type::event_type_set,
            Event
        >;

        if constexpr(can_process_event)
        {
            MAKI_AOS_CALL impl_type::template MAKI_AOS_NAME(call_internal_action)
            <
                AosType,
                Dry
            >
            (
                mach,
                ctx,
                event
            );

            MAKI_AOS_CALL tlu::MAKI_AOS_NAME(for_each)
            <
                AosType<void>,
                region_mix_type,
                region_process_event<AosType<int>, Dry>
            >(self, mach, ctx, event);

            MAKI_AOS_RETURN true;
        }
        else
        {
            const auto processed_count = MAKI_AOS_CALL tlu::MAKI_AOS_NAME(for_each_plus)
            <
                AosType<int>,
                region_mix_type,
                region_process_event<AosType<int>, Dry>
            >(self, mach, ctx, event);

            MAKI_AOS_RETURN static_cast<bool>(processed_count);
        }
    }

    region_mix_type regions_;
};

} //namespace
