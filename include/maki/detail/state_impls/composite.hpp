//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_IMPLS_COMPOSITE_HPP
#define MAKI_DETAIL_STATE_IMPLS_COMPOSITE_HPP

#include "simple.hpp"
#include "../type_set.hpp"
#include "../region_impl.hpp"
#include "../integer_constant_sequence.hpp"
#include "../mix.hpp"
#include "../friendly_impl.hpp"
#include "../tlu/apply.hpp"
#include "../tlu/left_fold.hpp"
#include "../tlu/for_each_plus.hpp"
#include "../tlu/for_each.hpp"
#include "../tlu/get.hpp"
#include "../tlu/size.hpp"
#include "../../state_mold.hpp"
#include "../../states.hpp"
#include "../../region.hpp"
#include <type_traits>
#include <utility>

namespace maki::detail::state_impls
{

template
<
    class ParentSm,
    const auto& ParentPath,
    int Index
>
struct region_mix_elem
{
    static constexpr auto transition_table = tuple_get<Index>(impl_of(ParentSm::mold).transition_tables);
    static constexpr auto path = ParentPath.add_region_index(Index);
    using type = region<region_impl<transition_table, path>>;
};

template
<
    class ParentSm,
    const auto& ParentPath,
    int Index
>
using region_mix_elem_t = typename region_mix_elem<ParentSm, ParentPath, Index>::type;

template
<
    class ParentSm,
    const auto& ParentPath,
    class RegionIndexSequence
>
struct region_mix;

template
<
    class ParentSm,
    const auto& ParentPath,
    int... RegionIndexes
>
struct region_mix
<
    ParentSm,
    ParentPath,
    std::integer_sequence<int, RegionIndexes...>
>
{
    using type = mix
    <
        region_mix_elem_t
        <
            ParentSm,
            ParentPath,
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

struct composite_from_context_args_tag_t{};
constexpr composite_from_context_args_tag_t composite_from_context_args_tag;

template<auto Id, const auto& Path>
class composite
{
public:
    static constexpr auto identifier = Id;
    static constexpr const auto& mold = *Id;
    using mold_type = std::decay_t<decltype(mold)>;
    using option_set_type = std::decay_t<decltype(impl_of(mold))>;
    using transition_table_type_list = decltype(impl_of(mold).transition_tables);
    using context_type = typename option_set_type::context_type;
    using impl_type = simple<identifier>;

    using region_index_sequence_type = std::make_integer_sequence
    <
        int,
        tlu::size_v<transition_table_type_list>
    >;

    using region_index_constant_sequence = make_integer_constant_sequence
    <
        int,
        tlu::size_v<transition_table_type_list>
    >;

    using region_mix_type = typename region_mix
    <
        composite,
        Path,
        region_index_sequence_type
    >::type;

    using event_type_set = type_set_union_t
    <
        typename impl_type::event_type_set,
        region_type_list_event_type_set<region_mix_type>
    >;

    template<class Machine, class... ContextArgs>
    composite
    (
        Machine& mach,
        composite_from_context_args_tag_t /*tag*/,
        ContextArgs&&... ctx_args
    ):
        impl_(mach, std::forward<ContextArgs>(ctx_args)...),
        regions_(mix_uniform_construct, mach, context())
    {
    }

    template<class Machine, class ParentContext>
    composite(Machine& mach, ParentContext& parent_ctx):
        impl_(mach, parent_ctx),
        regions_(mix_uniform_construct, mach, impl_.context_or(parent_ctx))
    {
    }

    composite(const composite&) = delete;
    composite(composite&&) = delete;
    composite& operator=(const composite&) = delete;
    composite& operator=(composite&&) = delete;
    ~composite() = default;

    auto& context()
    {
        return impl_.context();
    }

    const auto& context() const
    {
        return impl_.context();
    }

    template<class Machine, class ParentContext, class Event>
    void call_entry_action(Machine& mach, ParentContext& parent_ctx, const Event& event)
    {
        auto& ctx = impl_.context_or(parent_ctx);
        impl_.call_entry_action(mach, ctx, event);
        tlu::for_each<region_mix_type, region_enter>(*this, mach, ctx, event);
    }

    template<bool Dry, class Machine, class ParentContext, class Event>
    bool call_internal_action
    (
        Machine& mach,
        ParentContext& parent_ctx,
        const Event& event
    )
    {
        auto& ctx = impl_.context_or(parent_ctx);
        return call_internal_action_2<Dry>(*this, mach, ctx, event);
    }

    template<bool Dry, class Machine, class ParentContext, class Event>
    bool call_internal_action
    (
        Machine& mach,
        ParentContext& parent_ctx,
        const Event& event
    ) const
    {
        auto& ctx = impl_.context_or(parent_ctx);
        return call_internal_action_2<Dry>(*this, mach, ctx, event);
    }

    template<class Machine, class ParentContext, class Event>
    void call_exit_action
    (
        Machine& mach,
        ParentContext& parent_ctx,
        const Event& event
    )
    {
        auto& ctx = impl_.context_or(parent_ctx);
        tlu::for_each<region_mix_type, region_exit<&state_molds::null>>
        (
            *this,
            mach,
            ctx,
            event
        );
        impl_.call_exit_action
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
        tlu::for_each<region_mix_type, region_exit<&state_molds::fin>>
        (
            *this,
            mach,
            ctx,
            event
        );
        impl_.call_exit_action
        (
            mach,
            ctx,
            event
        );
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

    struct region_enter
    {
        template<class Region, class Self, class Machine, class Context, class Event>
        static void call(Self& self, Machine& mach, Context& ctx, const Event& event)
        {
            impl_of(get<Region>(self.regions_)).enter(mach, ctx, event);
        }
    };

    template<bool Dry>
    struct region_process_event
    {
        template<class Region, class Self, class Machine, class Context, class Event>
        static int call(Self& self, Machine& mach, Context& ctx, const Event& event)
        {
            const auto processed = impl_of(get<Region>(self.regions_)).template process_event<Dry>(mach, ctx, event);
            return static_cast<int>(processed);
        }
    };

    template<auto TargetStateId>
    struct region_exit
    {
        template<class Region, class Self, class Machine, class Context, class Event>
        static void call(Self& self, Machine& mach, Context& ctx, const Event& event)
        {
            impl_of(get<Region>(self.regions_)).template exit<TargetStateId>(mach, ctx, event);
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
        constexpr auto can_process_event = type_set_contains_v
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

            tlu::for_each<region_mix_type, region_process_event<Dry>>(self, mach, ctx, event);

            return true;
        }
        else
        {
            const auto processed_count = tlu::for_each_plus<region_mix_type, region_process_event<Dry>>(self, mach, ctx, event);
            return static_cast<bool>(processed_count);
        }
    }

    impl_type impl_;
    region_mix_type regions_;
};

} //namespace

#endif
