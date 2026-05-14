//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_IMPLS_COMPOSITE_NO_CONTEXT_HPP
#define MAKI_DETAIL_STATE_IMPLS_COMPOSITE_NO_CONTEXT_HPP

#include "simple_no_context.hpp"
#include "../state_mold_ids.hpp"
#include "../type_set.hpp"
#include "../region_impl.hpp"
#include "../context_storage.hpp"
#include "../iseq.hpp"
#include "../mix.hpp"
#include "../friendly_impl.hpp"
#include "../machine_fwd.hpp"
#include "../tlu/apply.hpp"
#include "../tlu/left_fold.hpp"
#include "../tlu/for_each_plus.hpp"
#include "../tlu/for_each.hpp"
#include "../tlu/get.hpp"
#include "../../region.hpp"
#include "../../context.hpp"
#include <type_traits>

namespace maki::detail::state_impls
{

template
<
    class MachineConfHolder,
    class ParentStateMoldPath,
    context_storage ParentCtxStorage,
    int Index
>
struct region_mix_elem
{
    using transition_table_path = iseq_push_back_t<ParentStateMoldPath, Index>;
    using type = region<region_impl<MachineConfHolder, transition_table_path, ParentCtxStorage>>;
};

template
<
    class MachineConfHolder,
    class ParentStateMoldPath,
    context_storage ParentCtxStorage,
    int Index
>
using region_mix_elem_t = typename region_mix_elem<MachineConfHolder, ParentStateMoldPath, ParentCtxStorage, Index>::type;

template
<
    class MachineConfHolder,
    class ParentStateMoldPath,
    context_storage ParentCtxStorage,
    class RegionIndexSequence
>
struct region_mix;

template
<
    class MachineConfHolder,
    class ParentStateMoldPath,
    context_storage ParentCtxStorage,
    int... RegionIndexes
>
struct region_mix
<
    MachineConfHolder,
    ParentStateMoldPath,
    ParentCtxStorage,
    iseq<RegionIndexes...>
>
{
    using type = mix
    <
        region_mix_elem_t
        <
            MachineConfHolder,
            ParentStateMoldPath,
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

template<class EventTypeSet, class Region>
using region_type_list_deferrable_event_type_set_operation =
    type_set_union_t
    <
        EventTypeSet,
        typename impl_of_t<Region>::deferrable_event_type_set
    >
;

template<class RegionTypeList>
using region_type_list_deferrable_event_type_set = tlu::left_fold_t
<
    RegionTypeList,
    region_type_list_deferrable_event_type_set_operation,
    empty_type_set_t
>;

template<class MachineConfHolder, class StateMoldPath, context_storage ParentCtxStorage>
class composite_no_context
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

    using region_mix_type = typename region_mix
    <
        MachineConfHolder,
        StateMoldPath,
        ctx_storage,
        region_index_sequence
    >::type;

    using event_type_set = type_set_union_t
    <
        typename impl_type::event_type_set,
        region_type_list_event_type_set<region_mix_type>
    >;

    using deferrable_event_type_set = type_set_union_t
    <
        typename impl_type::deferrable_event_type_set,
        region_type_list_deferrable_event_type_set<region_mix_type>
    >;

    template<class Context>
    composite_no_context(machine<MachineConfHolder>& mach, Context& ctx):
        regions_(mix_uniform_construct, mach, ctx)
    {
    }

    composite_no_context(const composite_no_context&) = delete;
    composite_no_context(composite_no_context&&) = delete;
    composite_no_context& operator=(const composite_no_context&) = delete;
    composite_no_context& operator=(composite_no_context&&) = delete;
    ~composite_no_context() = default;

    template<class Context>
    void emplace_contexts_with_parent_lifetime(Context& ctx, machine<MachineConfHolder>& mach)
    {
        tlu::for_each
        <
            region_mix_type,
            region_emplace_contexts_with_parent_lifetime
        >(*this, ctx, mach);
    }

    template<class Context, class Event>
    void enter(machine<MachineConfHolder>& mach, Context& ctx, const Event& event)
    {
        impl_type::enter(mach, ctx, event);
        tlu::for_each<region_mix_type, region_enter>(*this, mach, ctx, event);
    }

    template<bool Dry, class Context, class Event>
    bool call_internal_action
    (
        machine<MachineConfHolder>& mach,
        Context& ctx,
        const Event& event
    )
    {
        return call_internal_action_2<Dry>(*this, mach, ctx, event);
    }

    template<bool Dry, class Context, class Event>
    bool call_internal_action
    (
        const machine<MachineConfHolder>& mach,
        Context& ctx,
        const Event& event
    ) const
    {
        return call_internal_action_2<Dry>(*this, mach, ctx, event);
    }

    template<class Context, class Event>
    void exit(machine<MachineConfHolder>& mach, Context& ctx, const Event& event)
    {
        tlu::for_each<region_mix_type, region_exit<state_mold_ids::null>>
        (
            *this,
            mach,
            ctx,
            event
        );
        impl_type::exit
        (
            mach,
            ctx,
            event
        );
    }

    // For each region, transition from active state to final state.
    template<class Context, class Event>
    void exit_to_finals(machine<MachineConfHolder>& mach, Context& ctx, const Event& event)
    {
        tlu::for_each<region_mix_type, region_exit<state_mold_ids::fin>>
        (
            *this,
            mach,
            ctx,
            event
        );
        impl_type::exit
        (
            mach,
            ctx,
            event
        );
    }

    void reset_contexts_with_parent_lifetime()
    {
        tlu::for_each
        <
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

    struct region_enter
    {
        template<class Region, class Self, class Context, class Event>
        static void call(Self& self, machine<MachineConfHolder>& mach, Context& ctx, const Event& event)
        {
            impl_of(get<Region>(self.regions_)).enter(mach, ctx, event);
        }
    };

    template<bool Dry>
    struct region_process_event
    {
        template<class Region, class Self, class MachineArg, class Context, class Event>
        static int call(Self& self, MachineArg& mach, Context& ctx, const Event& event)
        {
            const auto processed = impl_of(get<Region>(self.regions_)).template process_event<Dry>(mach, ctx, event);
            return static_cast<int>(processed);
        }
    };

    template<int TargetStateMoldId>
    struct region_exit
    {
        template<class Region, class Self, class Context, class Event>
        static void call(Self& self, machine<MachineConfHolder>& mach, Context& ctx, const Event& event)
        {
            impl_of(get<Region>(self.regions_)).template exit<TargetStateMoldId>(mach, ctx, event);
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
        static bool call(const composite_no_context& self)
        {
            return impl_of(get<Region>(self.regions_)).template defers_event<Event>();
        }
    };

    template<bool Dry, class Self, class MachineArg, class Context, class Event>
    static bool call_internal_action_2
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
            impl_type::template call_internal_action<Dry>
            (
                mach,
                ctx,
                event
            );

            tlu::for_each<region_mix_type, region_process_event<Dry>>(self, mach, ctx, event);

            return true;
        }
        else
        {
            const auto processed_count = tlu::for_each_plus<region_mix_type, region_process_event<Dry>>(self, mach, ctx, event);
            return static_cast<bool>(processed_count);
        }
    }

    region_mix_type regions_;
};

} //namespace

#endif
