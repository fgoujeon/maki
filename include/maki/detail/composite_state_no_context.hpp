//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_COMPOSITE_STATE_NO_CONTEXT_HPP
#define MAKI_DETAIL_COMPOSITE_STATE_NO_CONTEXT_HPP

#include "maybe_bool_util.hpp"
#include "region.hpp"
#include "simple_state_no_context.hpp"
#include "tlu.hpp"
#include "tuple.hpp"
#include "../machine_conf.hpp"
#include "../state_conf.hpp"
#include "../region_proxy.hpp"
#include <type_traits>
#include <utility>

namespace maki::detail
{

template
<
    class ParentSm,
    const auto& ParentPath,
    int Index
>
struct region_tuple_elem
{
    static constexpr auto transition_table = tuple_get<Index>(opts(ParentSm::conf).transition_tables);
    static constexpr auto path = ParentPath.add_region_index(Index);
    using type = region_proxy<region<transition_table, path>>;
};

template
<
    class ParentSm,
    const auto& ParentPath,
    int Index
>
using region_tuple_elem_t = typename region_tuple_elem<ParentSm, ParentPath, Index>::type;

template
<
    class ParentSm,
    const auto& ParentPath,
    class RegionIndexSequence
>
struct region_tuple;

template
<
    class ParentSm,
    const auto& ParentPath,
    int... RegionIndexes
>
struct region_tuple
<
    ParentSm,
    ParentPath,
    std::integer_sequence<int, RegionIndexes...>
>
{
    using type = tuple
    <
        region_tuple_elem_t
        <
            ParentSm,
            ParentPath,
            RegionIndexes
        >...
    >;
};

template<auto Id, const auto& Path>
class composite_state_no_context
{
public:
    static constexpr auto identifier = Id;
    static constexpr const auto& conf = *Id;
    using conf_type = std::decay_t<decltype(conf)>;
    using option_set_type = std::decay_t<decltype(opts(conf))>;
    using transition_table_type_list = decltype(opts(conf).transition_tables);

    template<class Machine, class Context>
    composite_state_no_context(Machine& mach, Context& ctx):
        regions_(uniform_construct, mach, ctx)
    {
    }

    composite_state_no_context(const composite_state_no_context&) = delete;
    composite_state_no_context(composite_state_no_context&&) = delete;
    composite_state_no_context& operator=(const composite_state_no_context&) = delete;
    composite_state_no_context& operator=(composite_state_no_context&&) = delete;
    ~composite_state_no_context() = default;

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

    template<int Index>
    [[nodiscard]] const auto& region() const
    {
        return tuple_get<Index>(regions_);
    }

    template<const auto& StateConf>
    [[nodiscard]] const auto& state() const
    {
        static_assert(region_tuple_type::size == 1);
        return region<0>().impl().template state<StateConf>();
    }

    template<const auto& StateConf>
    [[nodiscard]] bool is() const
    {
        static_assert(region_tuple_type::size == 1);
        return region<0>().impl().template is<StateConf>();
    }

    [[nodiscard]] bool running() const
    {
        static_assert(region_tuple_type::size == 1);
        return region<0>().impl().running();
    }

    template<class /*Event*/>
    static constexpr bool has_internal_action_for_event()
    {
        return true;
    }

private:
    using impl_type = simple_state_no_context<Id>;

    using region_tuple_type = typename region_tuple
    <
        composite_state_no_context,
        Path,
        std::make_integer_sequence<int, tlu::size_v<transition_table_type_list>>
    >::type;

    struct region_start
    {
        template<class Region, class Self, class Machine, class Context, class Event>
        static void call(Self& self, Machine& mach, Context& ctx, const Event& event)
        {
            tuple_get<Region>(self.regions_).impl().start(mach, ctx, event);
        }
    };

    template<bool Dry>
    struct region_process_event
    {
        template<class Region, class Self, class Machine, class Context, class Event, class... MaybeBool>
        static void call(Self& self, Machine& mach, Context& ctx, const Event& event, MaybeBool&... processed)
        {
            tuple_get<Region>(self.regions_).impl().template process_event<Dry>(mach, ctx, event, processed...);
        }
    };

    struct region_stop
    {
        template<class Region, class Self, class Machine, class Context, class Event>
        static void call(Self& self, Machine& mach, Context& ctx, const Event& event)
        {
            tuple_get<Region>(self.regions_).impl().stop(mach, ctx, event);
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
