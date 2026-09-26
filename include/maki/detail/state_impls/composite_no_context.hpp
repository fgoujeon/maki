// Copyright Florian Goujeon 2021 - 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
// Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_IMPLS_COMPOSITE_NO_CONTEXT_HPP
#define MAKI_DETAIL_STATE_IMPLS_COMPOSITE_NO_CONTEXT_HPP

#include "../../context.hpp"
#include "../../region.hpp"
#include "../context_storage.hpp"
#include "../context_tree.hpp"
#include "../friendly_impl.hpp"
#include "../iseq.hpp"
#include "../machine_fwd.hpp"
#include "../mix.hpp"
#include "../region_impl.hpp"
#include "../state_mold_ids.hpp"
#include "../tlu/apply.hpp"
#include "../tlu/for_each.hpp"
#include "../tlu/for_each_plus.hpp"
#include "../tlu/get.hpp"
#include "../tlu/left_fold.hpp"
#include "../type_set.hpp"
#include "simple_no_context.hpp"
#include <type_traits>

namespace maki::detail::state_impls
{

template<class MachineConfHolder, class ParentStateMoldPath, int Index>
struct region_mix_elem
{
    using transition_table_path = iseq_push_back_t<ParentStateMoldPath, Index>;
    using type = region<region_impl<MachineConfHolder, transition_table_path>>;
};

template<class MachineConfHolder, class ParentStateMoldPath, int Index>
using region_mix_elem_t =
    typename region_mix_elem<MachineConfHolder, ParentStateMoldPath, Index>::
        type;

template<
    class MachineConfHolder,
    class ParentStateMoldPath,
    class RegionIndexSequence>
struct region_mix;

template<
    class MachineConfHolder,
    class ParentStateMoldPath,
    int... RegionIndexes>
struct region_mix<
    MachineConfHolder,
    ParentStateMoldPath,
    iseq<RegionIndexes...>>
{
    using type = mix<region_mix_elem_t<
        MachineConfHolder,
        ParentStateMoldPath,
        RegionIndexes>...>;
};

template<class EventTypeSet, class Region>
using region_type_list_event_type_set_operation =
    type_set_union_t<EventTypeSet, typename impl_of_t<Region>::event_type_set>;

template<class RegionTypeList>
using region_type_list_event_type_set = tlu::left_fold_t<
    region_type_list_event_type_set_operation,
    empty_type_set_t,
    RegionTypeList>;

template<class EventTypeSet, class Region>
using region_type_list_deferrable_event_type_set_operation = type_set_union_t<
    EventTypeSet,
    typename impl_of_t<Region>::deferrable_event_type_set>;

template<class RegionTypeList>
using region_type_list_deferrable_event_type_set = tlu::left_fold_t<
    region_type_list_deferrable_event_type_set_operation,
    empty_type_set_t,
    RegionTypeList>;

template<class MachineConfHolder, class StateMoldPath, int TransitionTableCount>
class composite_no_context
{
public:
    using machine_conf_holder_type = MachineConfHolder;
    using state_mold_path = StateMoldPath;

    static constexpr const auto& mold =
        machine_conf_tree::node_at_path_v<MachineConfHolder, StateMoldPath>;
    using mold_type = std::decay_t<decltype(mold)>;
    using option_set_type = std::decay_t<decltype(impl_of(mold))>;
    using transition_table_type_list =
        decltype(impl_of(mold).transition_tables);
    using impl_type = simple_no_context<MachineConfHolder, StateMoldPath>;

    using region_index_sequence =
        linear_iseq_t<impl_of(mold).transition_tables.size>;

    using region_mix_type = typename region_mix<
        MachineConfHolder,
        StateMoldPath,
        region_index_sequence>::type;

    // Events of interest to the substates
    using substates_event_type_set =
        region_type_list_event_type_set<region_mix_type>;

    // Events of interest to the internal actions
    using internal_actions_event_type_set = typename impl_type::event_type_set;

    using event_type_set = type_set_union_t<
        internal_actions_event_type_set,
        substates_event_type_set>;

    using deferrable_event_type_set = type_set_union_t<
        typename impl_type::deferrable_event_type_set,
        region_type_list_deferrable_event_type_set<region_mix_type>>;

    composite_no_context(context_tree<MachineConfHolder>& ctx_tree):
        ctx_tree_(ctx_tree), regions_(mix_uniform_construct, ctx_tree)
    {
    }

    composite_no_context(const composite_no_context&) = delete;
    composite_no_context(composite_no_context&&) = delete;
    composite_no_context& operator=(const composite_no_context&) = delete;
    composite_no_context& operator=(composite_no_context&&) = delete;
    ~composite_no_context() = default;

    auto& context()
    {
        return ctx_tree_.template opt_context_at<StateMoldPath>();
    }

    const auto& context() const
    {
        return ctx_tree_.template opt_context_at<StateMoldPath>();
    }

    template<class Context, class Event>
    void
    enter(machine<MachineConfHolder>& mach, Context& ctx, const Event& event)
    {
        impl_type::enter(mach, ctx, event);
        tlu::for_each<region_mix_type, region_enter>(*this, mach, ctx, event);
    }

    template<class Context, class Event>
    bool process_event(
        machine<MachineConfHolder>& mach,
        Context& ctx,
        const Event& event)
    {
        return call_internal_action_2<false>(*this, mach, ctx, event);
    }

    template<class Context, class Event>
    bool check_event(
        const machine<MachineConfHolder>& mach,
        Context& ctx,
        const Event& event) const
    {
        return call_internal_action_2<true>(*this, mach, ctx, event);
    }

    template<class Context, class Event>
    void
    exit(machine<MachineConfHolder>& mach, Context& ctx, const Event& event)
    {
        tlu::for_each<region_mix_type, region_exit<state_mold_ids::null>>(
            *this,
            mach,
            ctx,
            event);

        impl_type::exit(mach, ctx, event);
    }

    // For each region, transition from active state to final state.
    template<class Context, class Event>
    void exit_to_finals(
        machine<MachineConfHolder>& mach,
        Context& ctx,
        const Event& event)
    {
        tlu::for_each<region_mix_type, region_exit<state_mold_ids::fin>>(
            *this,
            mach,
            ctx,
            event);

        impl_type::exit(mach, ctx, event);
    }

    template<class Event>
    [[nodiscard]] bool defers_event() const
    {
        if constexpr (type_set_contains_v<deferrable_event_type_set, Event>)
        {
            return impl_type::template defers_event<Event>() ||
                tlu::for_each_or<region_mix_type, region_defers_event<Event>>(
                    *this);
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
        return tlu::apply_t<all_regions_completed, region_mix_type>::call(
            *this);
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

    struct region_enter
    {
        template<class Region, class Self, class Context, class Event>
        static void call(
            Self& self,
            machine<MachineConfHolder>& mach,
            Context& ctx,
            const Event& event)
        {
            impl_of(get<Region>(self.regions_)).enter(mach, ctx, event);
        }
    };

    template<bool Dry>
    struct region_process_event
    {
        template<
            class Region,
            class Self,
            class MachineArg,
            class Context,
            class Event>
        static int
        call(Self& self, MachineArg& mach, Context& ctx, const Event& event)
        {
            const auto processed =
                impl_of(get<Region>(self.regions_))
                    .template process_event<Dry>(mach, ctx, event);
            return static_cast<int>(processed);
        }
    };

    template<int TargetStateMoldId>
    struct region_exit
    {
        template<class Region, class Self, class Context, class Event>
        static void call(
            Self& self,
            machine<MachineConfHolder>& mach,
            Context& ctx,
            const Event& event)
        {
            impl_of(get<Region>(self.regions_))
                .template exit<TargetStateMoldId>(mach, ctx, event);
        }
    };

    template<class Event>
    struct region_defers_event
    {
        template<class Region>
        static bool call(const composite_no_context& self)
        {
            return impl_of(get<Region>(self.regions_))
                .template defers_event<Event>();
        }
    };

    template<bool Dry, class Self, class MachineArg, class Context, class Event>
    static bool call_internal_action_2(
        Self& self,
        MachineArg& mach,
        Context& ctx,
        const Event& event)
    {
        constexpr auto can_process_event_in_substates =
            type_set_contains_v<substates_event_type_set, Event>;

        constexpr auto can_process_event_in_internal_actions =
            type_set_contains_v<internal_actions_event_type_set, Event>;

        if constexpr (can_process_event_in_substates &&
            !can_process_event_in_internal_actions)
        {
            return process_event_in_substates<Dry>(self, mach, ctx, event);
        }
        else if constexpr (!can_process_event_in_substates &&
            can_process_event_in_internal_actions)
        {
            return process_event_in_internal_actions<Dry>(mach, ctx, event);
        }
        else if constexpr (can_process_event_in_substates &&
            can_process_event_in_internal_actions)
        {
            // Substates take priority as they're deeper in the hierarchy.
            return process_event_in_substates<Dry>(self, mach, ctx, event) ||
                process_event_in_internal_actions<Dry>(mach, ctx, event);
        }
        else
        {
            return false;
        }
    }

    template<bool Dry, class Self, class MachineArg, class Context, class Event>
    static bool process_event_in_substates(
        Self& self,
        MachineArg& mach,
        Context& ctx,
        const Event& event)
    {
        const auto processed_count =
            tlu::for_each_plus<region_mix_type, region_process_event<Dry>>(
                self,
                mach,
                ctx,
                event);
        return static_cast<bool>(processed_count);
    }

    template<bool Dry, class MachineArg, class Context, class Event>
    static bool process_event_in_internal_actions(
        MachineArg& mach,
        Context& ctx,
        const Event& event)
    {
        if constexpr (Dry)
        {
            return impl_type::check_event(mach, ctx, event);
        }
        else
        {
            return impl_type::process_event(mach, ctx, event);
        }
    }

    context_tree<MachineConfHolder>& ctx_tree_;
    region_mix_type regions_;
};

// Specialization for one region, to avoid `mix` machinery.
template<class MachineConfHolder, class StateMoldPath>
class composite_no_context<MachineConfHolder, StateMoldPath, 1>
{
public:
    using machine_conf_holder_type = MachineConfHolder;
    using state_mold_path = StateMoldPath;

    static constexpr const auto& mold =
        machine_conf_tree::node_at_path_v<MachineConfHolder, StateMoldPath>;
    using mold_type = std::decay_t<decltype(mold)>;
    using option_set_type = std::decay_t<decltype(impl_of(mold))>;
    using transition_table_type_list =
        decltype(impl_of(mold).transition_tables);
    using impl_type = simple_no_context<MachineConfHolder, StateMoldPath>;

    using region_impl_type =
        region_impl<MachineConfHolder, iseq_push_back_t<StateMoldPath, 0>>;

    // Events of interest to the substates
    using substates_event_type_set = typename region_impl_type::event_type_set;

    // Events of interest to the internal actions
    using internal_actions_event_type_set = typename impl_type::event_type_set;

    using event_type_set = type_set_union_t<
        internal_actions_event_type_set,
        substates_event_type_set>;

    using deferrable_event_type_set =
        typename region_impl_type::deferrable_event_type_set;

    composite_no_context(context_tree<MachineConfHolder>& ctx_tree):
        ctx_tree_(ctx_tree), region_(ctx_tree)
    {
    }

    composite_no_context(const composite_no_context&) = delete;
    composite_no_context(composite_no_context&&) = delete;
    composite_no_context& operator=(const composite_no_context&) = delete;
    composite_no_context& operator=(composite_no_context&&) = delete;
    ~composite_no_context() = default;

    auto& context()
    {
        return ctx_tree_.template opt_context_at<StateMoldPath>();
    }

    const auto& context() const
    {
        return ctx_tree_.template opt_context_at<StateMoldPath>();
    }

    template<class Context, class Event>
    void
    enter(machine<MachineConfHolder>& mach, Context& ctx, const Event& event)
    {
        impl_type::enter(mach, ctx, event);
        impl_of(region_).enter(mach, ctx, event);
    }

    template<class Context, class Event>
    bool process_event(
        machine<MachineConfHolder>& mach,
        Context& ctx,
        const Event& event)
    {
        return call_internal_action_2<false>(*this, mach, ctx, event);
    }

    template<class Context, class Event>
    bool check_event(
        const machine<MachineConfHolder>& mach,
        Context& ctx,
        const Event& event) const
    {
        return call_internal_action_2<true>(*this, mach, ctx, event);
    }

    template<class Context, class Event>
    void
    exit(machine<MachineConfHolder>& mach, Context& ctx, const Event& event)
    {
        impl_of(region_).template exit<state_mold_ids::null>(mach, ctx, event);
        impl_type::exit(mach, ctx, event);
    }

    // For each region, transition from active state to final state.
    template<class Context, class Event>
    void exit_to_finals(
        machine<MachineConfHolder>& mach,
        Context& ctx,
        const Event& event)
    {
        impl_of(region_).template exit<state_mold_ids::fin>(mach, ctx, event);
        impl_type::exit(mach, ctx, event);
    }

    template<class Event>
    [[nodiscard]] bool defers_event() const
    {
        if constexpr (type_set_contains_v<deferrable_event_type_set, Event>)
        {
            return impl_of(region_).template defers_event<Event>();
        }
        else
        {
            return false;
        }
    }

    template<int Index>
    [[nodiscard]] const auto& region() const
    {
        static_assert(Index == 0);
        return region_;
    }

    template<const auto& StateMold>
    [[nodiscard]] const auto& state() const
    {
        return impl_of(region_).template state<StateMold>();
    }

    template<const auto& StateMold>
    [[nodiscard]] bool is() const
    {
        return impl_of(region_).template is<StateMold>();
    }

    [[nodiscard]] bool completed() const
    {
        return impl_of(region_).completed();
    }

private:
    template<bool Dry>
    struct region_process_event
    {
        template<
            class Region,
            class Self,
            class MachineArg,
            class Context,
            class Event>
        static int
        call(Self& self, MachineArg& mach, Context& ctx, const Event& event)
        {
            const auto processed =
                impl_of(get<Region>(self.regions_))
                    .template process_event<Dry>(mach, ctx, event);
            return static_cast<int>(processed);
        }
    };

    template<bool Dry, class Self, class MachineArg, class Context, class Event>
    static bool call_internal_action_2(
        Self& self,
        MachineArg& mach,
        Context& ctx,
        const Event& event)
    {
        constexpr auto can_process_event_in_substates =
            type_set_contains_v<substates_event_type_set, Event>;

        constexpr auto can_process_event_in_internal_actions =
            type_set_contains_v<internal_actions_event_type_set, Event>;

        if constexpr (can_process_event_in_substates &&
            !can_process_event_in_internal_actions)
        {
            return impl_of(self.region_)
                .template process_event<Dry>(mach, ctx, event);
        }
        else if constexpr (!can_process_event_in_substates &&
            can_process_event_in_internal_actions)
        {
            return process_event_in_internal_actions<Dry>(mach, ctx, event);
        }
        else if constexpr (can_process_event_in_substates &&
            can_process_event_in_internal_actions)
        {
            // Substates take priority as they're deeper in the hierarchy.
            return impl_of(self.region_)
                       .template process_event<Dry>(mach, ctx, event) ||
                process_event_in_internal_actions<Dry>(mach, ctx, event);
        }
        else
        {
            return false;
        }
    }

    template<bool Dry, class MachineArg, class Context, class Event>
    static bool process_event_in_internal_actions(
        MachineArg& mach,
        Context& ctx,
        const Event& event)
    {
        if constexpr (Dry)
        {
            return impl_type::check_event(mach, ctx, event);
        }
        else
        {
            return impl_type::process_event(mach, ctx, event);
        }
    }

    context_tree<MachineConfHolder>& ctx_tree_;
    maki::region<region_impl_type> region_;
};

} // namespace maki::detail::state_impls

#endif
