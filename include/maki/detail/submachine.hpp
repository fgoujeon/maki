//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_SUBMACHINE_HPP
#define MAKI_DETAIL_SUBMACHINE_HPP

#include "call_member.hpp"
#include "tlu.hpp"
#include "region.hpp"
#include "maybe_bool_util.hpp"
#include "simple_state.hpp"
#include "submachine_fwd.hpp"
#include "state_traits.hpp"
#include "tuple.hpp"
#include "../machine_fwd.hpp"
#include "../state_conf.hpp"
#include "../machine_conf.hpp"
#include "../transition_table.hpp"
#include "../path.hpp"
#include "../type_patterns.hpp"
#include <type_traits>

namespace maki::detail
{

template
<
    class ParentSm,
    class RegionIndexSequence
>
struct region_tuple;

template
<
    class ParentSm,
    int... RegionIndexes
>
struct region_tuple
<
    ParentSm,
    std::integer_sequence<int, RegionIndexes...>
>
{
    using type = tuple
    <
        region
        <
            ParentSm,
            RegionIndexes
        >...
    >;
};

template<const auto& Conf, class ParentRegion, class ContextType>
class submachine_impl
{
public:
    using conf_type = std::decay_t<decltype(Conf)>;
    using option_set_type = std::decay_t<decltype(opts(Conf))>;
    using transition_table_fn_tuple_list = decltype(opts(Conf).transition_table_fns);
    using context_type = ContextType;

    template
    <
        class Machine,
        class ParentContext,
        class ConfType = conf_type,
        std::enable_if_t<!is_root_sm_conf_v<ConfType>, bool> = true
    >
    submachine_impl(Machine& mach, ParentContext& parent_ctx):
        ctx_holder_(mach, parent_ctx),
        impl_(mach, context())
    {
    }

    auto& context()
    {
        return ctx_holder_.get();
    }

    const auto& context() const
    {
        return ctx_holder_.get();
    }

    template<class ParentContext>
    auto& context_or(ParentContext& /*parent_ctx*/)
    {
        return context();
    }

    template<class ParentContext>
    const auto& context_or(ParentContext& /*parent_ctx*/) const
    {
        return context();
    }

    template<const auto& StatePath, class ParentContext>
    auto& context_or(ParentContext& /*parent_ctx*/)
    {
        if constexpr(StatePath.empty())
        {
            return context();
        }
        else
        {
            return impl_.template context_or<StatePath>(context());
        }
    }

    template<const auto& StatePath, class ParentContext>
    const auto& context_or(ParentContext& /*parent_ctx*/) const
    {
        if constexpr(StatePath.empty())
        {
            return context();
        }
        else
        {
            return impl_.template context_or<StatePath>(context());
        }
    }

    template<const auto& StateRegionPath, const auto& StateConf>
    [[nodiscard]] bool active_state() const
    {
        return impl_.template active_state<StateRegionPath, StateConf>();
    }

    template<const auto& StateConf>
    [[nodiscard]] bool active_state() const
    {
        return impl_.template active_state<StateConf>();
    }

    template<const auto& RegionPath>
    [[nodiscard]] bool running() const
    {
        return impl_.template running<RegionPath>();
    }

    [[nodiscard]] bool running() const
    {
        return impl_.running();
    }

    template<class Machine, class ParentContext, class Event>
    void call_entry_action(Machine& mach, ParentContext& /*parent_ctx*/, const Event& event)
    {
        impl_.call_entry_action(mach, context(), event);
    }

    template<bool Dry = false, class Machine, class ParentContext, class Event>
    void call_internal_action
    (
        Machine& mach,
        ParentContext& /*parent_ctx*/,
        const Event& event
    )
    {
        impl_.call_internal_action(mach, context(), event);
    }

    template<bool Dry = false, class Machine, class ParentContext, class Event>
    void call_internal_action
    (
        Machine& mach,
        ParentContext& /*parent_ctx*/,
        const Event& event,
        bool& processed
    )
    {
        impl_.call_internal_action(mach, context(), event, processed);
    }

    template<bool Dry = false, class Machine, class ParentContext, class Event>
    void call_internal_action
    (
        Machine& mach,
        ParentContext& /*parent_ctx*/,
        const Event& event
    ) const
    {
        impl_.call_internal_action(mach, context(), event);
    }

    template<bool Dry = false, class Machine, class ParentContext, class Event>
    void call_internal_action
    (
        Machine& mach,
        ParentContext& /*parent_ctx*/,
        const Event& event,
        bool& processed
    ) const
    {
        impl_.call_internal_action(mach, context(), event, processed);
    }

    template<class Machine, class ParentContext, class Event>
    void call_exit_action
    (
        Machine& mach,
        ParentContext& /*parent_ctx*/,
        const Event& event
    )
    {
        impl_.call_exit_action(mach, context(), event);
    }

    template<class /*Event*/>
    static constexpr bool has_internal_action_for_event()
    {
        return true;
    }

    static constexpr const auto& conf = Conf;

private:
    using impl_type = submachine_impl<Conf, ParentRegion, void>;

    context_holder<context_type, opts(Conf).context_sig> ctx_holder_;
    impl_type impl_;
};

template<const auto& Conf, class ParentRegion>
class submachine_impl<Conf, ParentRegion, void>
{
public:
    using conf_type = std::decay_t<decltype(Conf)>;
    using option_set_type = std::decay_t<decltype(opts(Conf))>;
    using transition_table_fn_tuple_list = decltype(opts(Conf).transition_table_fns);

    template<class Machine, class Context>
    submachine_impl(Machine& mach, Context& ctx):
        regions_(uniform_construct, mach, ctx)
    {
    }

    template<const auto& StatePath, class ParentContext>
    auto& context_or(ParentContext& parent_ctx)
    {
        static constexpr int region_index = path_raw_head(StatePath);
        static constexpr auto state_path_tail = path_tail(StatePath);
        return tuple_get<region_index>(regions_).template context_or<state_path_tail>(parent_ctx);
    }

    template<const auto& StatePath, class ParentContext>
    const auto& context_or(ParentContext& parent_ctx) const
    {
        static constexpr int region_index = path_raw_head(StatePath);
        static constexpr auto state_path_tail = path_tail(StatePath);
        return tuple_get<region_index>(regions_).template context_or<state_path_tail>(parent_ctx);
    }

    template<const auto& StateRegionPath, const auto& StateConf>
    [[nodiscard]] bool active_state() const
    {
        static constexpr auto region_index = path_raw_head(StateRegionPath);
        static constexpr auto state_region_relative_path = path_tail(StateRegionPath);
        return tuple_get<region_index>(regions_).template active_state<state_region_relative_path, &StateConf>();
    }

    template<const auto& StateConf>
    [[nodiscard]] bool active_state() const
    {
        static_assert(tlu::size_v<transition_table_fn_tuple_list> == 1);

        static constexpr auto state_region_relative_path = path<>{};
        return tuple_get<0>(regions_).template active_state<state_region_relative_path, &StateConf>();
    }

    template<const auto& RegionPath>
    [[nodiscard]] bool running() const
    {
        return !active_state<RegionPath, state_confs::stopped>();
    }

    [[nodiscard]] bool running() const
    {
        return !active_state<state_confs::stopped>();
    }

    template<class Machine, class Context, class Event>
    void call_entry_action(Machine& mach, Context& ctx, const Event& event)
    {
        impl_.call_entry_action(mach, ctx, event);
        tlu::for_each<region_tuple_type, region_start>(*this, mach, ctx, event);
    }

    template<bool Dry = false, class Machine, class Context, class Event>
    void call_internal_action
    (
        Machine& mach,
        Context& ctx,
        const Event& event
    )
    {
        call_internal_action_2<Dry>(*this, mach, ctx, event);
    }

    template<bool Dry = false, class Machine, class Context, class Event>
    void call_internal_action
    (
        Machine& mach,
        Context& ctx,
        const Event& event,
        bool& processed
    )
    {
        call_internal_action_2<Dry>(*this, mach, ctx, event, processed);
    }

    template<bool Dry = false, class Machine, class Context, class Event>
    void call_internal_action
    (
        Machine& mach,
        Context& ctx,
        const Event& event
    ) const
    {
        call_internal_action_2<Dry>(*this, mach, ctx, event);
    }

    template<bool Dry = false, class Machine, class Context, class Event>
    void call_internal_action
    (
        Machine& mach,
        Context& ctx,
        const Event& event,
        bool& processed
    ) const
    {
        call_internal_action_2<Dry>(*this, mach, ctx, event, processed);
    }

    template<class Machine, class Context, class Event>
    void call_exit_action(Machine& mach, Context& ctx, const Event& event)
    {
        tlu::for_each<region_tuple_type, region_stop>(*this, mach, ctx, event);
        impl_.call_exit_action
        (
            mach,
            ctx,
            event
        );
    }

    template<class /*Event*/>
    static constexpr bool has_internal_action_for_event()
    {
        return true;
    }

    static constexpr const auto& conf = Conf;

private:
    using impl_type = simple_state_impl<Conf, void>;

    using region_tuple_type = typename region_tuple
    <
        submachine_impl,
        std::make_integer_sequence<int, tlu::size_v<transition_table_fn_tuple_list>>
    >::type;

    struct region_start
    {
        template<class Region, class Self, class Machine, class Context, class Event>
        static void call(Self& self, Machine& mach, Context& ctx, const Event& event)
        {
            tuple_get<Region>(self.regions_).start(mach, ctx, event);
        }
    };

    template<bool Dry>
    struct region_process_event
    {
        template<class Region, class Self, class Machine, class Context, class Event, class... MaybeBool>
        static void call(Self& self, Machine& mach, Context& ctx, const Event& event, MaybeBool&... processed)
        {
            tuple_get<Region>(self.regions_).template process_event<Dry>(mach, ctx, event, processed...);
        }
    };

    struct region_stop
    {
        template<class Region, class Self, class Machine, class Context, class Event>
        static void call(Self& self, Machine& mach, Context& ctx, const Event& event)
        {
            tuple_get<Region>(self.regions_).stop(mach, ctx, event);
        }
    };

    template<bool Dry, class Self, class Machine, class Context, class Event, class... MaybeBool>
    static void call_internal_action_2
    (
        Self& self,
        Machine& mach,
        Context& ctx,
        const Event& event,
        MaybeBool&... processed
    )
    {
        if constexpr(impl_type::template has_internal_action_for_event<Event>())
        {
            if constexpr(!Dry)
            {
                self.impl_.call_internal_action
                (
                    mach,
                    ctx,
                    event
                );

                tlu::for_each<region_tuple_type, region_process_event<Dry>>(self, mach, ctx, event);
            }

            maybe_bool_util::set_to_true(processed...);
        }
        else
        {
            tlu::for_each<region_tuple_type, region_process_event<Dry>>(self, mach, ctx, event, processed...);
        }
    }

    impl_type impl_;
    region_tuple_type regions_;
};

} //namespace

#endif
