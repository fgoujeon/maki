//Copyright Florian Goujeon 2021 - 2023.
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
#include "machine_object_holder.hpp"
#include "context_holder.hpp"
#include "simple_state.hpp"
#include "submachine_fwd.hpp"
#include "tuple.hpp"
#include "../machine_fwd.hpp"
#include "../state_conf.hpp"
#include "../transition_table.hpp"
#include "../path.hpp"
#include "../type_patterns.hpp"
#include <type_traits>

namespace maki::detail
{

template<const auto& Conf, class ParentRegion>
struct submachine_context
{
    using option_set_type = std::decay_t<decltype(opts(Conf))>;
    using conf_context_type = typename option_set_type::context_type;

    /*
    Context type is either (in this order of priority):
    - the one specified in the submachine_opts::context option, if any;
    - a reference to the context type of the parent SM (not necessarily the root
      SM).
    */
    using type = std::conditional_t
    <
        std::is_void_v<conf_context_type>,
        typename ParentRegion::parent_sm_type::context_type&,
        conf_context_type
    >;
};

template<const auto& Conf>
struct submachine_context<Conf, void>
{
    using type = typename std::decay_t<decltype(opts(Conf))>::context_type;
};

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

template<const auto& Conf, class ParentRegion>
class submachine
{
public:
    using option_set_type = std::decay_t<decltype(opts(Conf))>;
    using context_type = typename submachine_context<Conf, ParentRegion>::type;
    using transition_table_type_list = decltype(opts(Conf).transition_tables);

    template<class Machine, class... ContextArgs>
    submachine(Machine& mach, ContextArgs&&... ctx_args):
        ctx_holder_(mach, std::forward<ContextArgs>(ctx_args)...),
        simple_state_(mach, context()),
        regions_(uniform_construct, mach, *this)
    {
    }

    context_type& context()
    {
        return ctx_holder_.get();
    }

    const context_type& context() const
    {
        return ctx_holder_.get();
    }

    auto& data()
    {
        return simple_state_.data();
    }

    const auto& data() const
    {
        return simple_state_.data();
    }

    template<const auto& StatePath>
    auto& data()
    {
        if constexpr(StatePath.empty())
        {
            return simple_state_.data();
        }
        else
        {
            static constexpr int region_index = path_raw_head(StatePath);
            static constexpr auto state_path_tail = path_tail(StatePath);
            return tuple_get<region_index>(regions_).template data<state_path_tail>();
        }
    }

    template<const auto& StatePath>
    const auto& data() const
    {
        if constexpr(StatePath.empty())
        {
            return simple_state_.data();
        }
        else
        {
            static constexpr int region_index = path_raw_head(StatePath);
            static constexpr auto state_path_tail = path_tail(StatePath);
            return tuple_get<region_index>(regions_).template data<state_path_tail>();
        }
    }

    template<const auto& StateRegionPath, const auto& StateConf>
    [[nodiscard]] bool active_state() const
    {
        static constexpr auto region_index = path_raw_head(StateRegionPath);
        static constexpr auto state_region_relative_path = path_tail(StateRegionPath);
        return tuple_get<region_index>(regions_).template active_state<state_region_relative_path, StateConf>();
    }

    template<const auto& StateConf>
    [[nodiscard]] bool active_state() const
    {
        static_assert(tlu::size_v<transition_table_type_list> == 1);

        static constexpr auto state_region_relative_path = path<>{};
        return tuple_get<0>(regions_).template active_state<state_region_relative_path, StateConf>();
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
        simple_state_.call_entry_action
        (
            mach,
            own_context_or(ctx),
            event
        );
        tlu::for_each<region_tuple_type, region_start>(*this, mach, own_context_or(ctx), event);
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
        tlu::for_each<region_tuple_type, region_stop>(*this, mach, own_context_or(ctx), event);
        simple_state_.call_exit_action
        (
            mach,
            own_context_or(ctx),
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
    using region_tuple_type = typename region_tuple
    <
        submachine,
        std::make_integer_sequence<int, tlu::size_v<transition_table_type_list>>
    >::type;

    //We need a simple_state to manage data and actions
    using simple_state_type = simple_state<Conf>;

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
        if constexpr(simple_state_type::template has_internal_action_for_event<Event>())
        {
            if constexpr(!Dry)
            {
                self.simple_state_.call_internal_action
                (
                    mach,
                    self.own_context_or(ctx),
                    event
                );

                tlu::for_each<region_tuple_type, region_process_event<Dry>>(self, mach, self.own_context_or(ctx), event);
            }

            maybe_bool_util::set_to_true(processed...);
        }
        else
        {
            tlu::for_each<region_tuple_type, region_process_event<Dry>>(self, mach, self.own_context_or(ctx), event, processed...);
        }
    }

    template<class Context>
    auto& own_context_or(Context& ctx)
    {
        if constexpr(std::is_void_v<typename option_set_type::context_type>)
        {
            return ctx;
        }
        else
        {
            return ctx_holder_.get();
        }
    }

    template<class Context>
    const auto& own_context_or(const Context& ctx) const
    {
        if constexpr(std::is_void_v<typename option_set_type::context_type>)
        {
            return ctx;
        }
        else
        {
            return ctx_holder_.get();
        }
    }

    context_holder<context_type> ctx_holder_;
    simple_state_type simple_state_;
    region_tuple_type regions_;
};

} //namespace

#endif
