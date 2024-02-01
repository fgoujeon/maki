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

template<const auto& Conf, class Parent>
class submachine
{
public:
    using conf_type = std::decay_t<decltype(Conf)>;
    using option_set_type = std::decay_t<decltype(opts(Conf))>;
    using transition_table_type_list = decltype(opts(Conf).transition_tables);
    using decayed_parent_context_type = decayed_context_of_t<Parent>;

    template
    <
        class Machine,
        class... ContextArgs,
        class ConfType = conf_type,
        std::enable_if_t<is_root_sm_conf_v<ConfType>, bool> = true
    >
    submachine(Machine& mach, ContextArgs&&... ctx_args):
        simple_state_(mach, std::forward<ContextArgs>(ctx_args)...),
        regions_(uniform_construct, mach, context())
    {
    }

    template
    <
        class Machine,
        class ConfType = conf_type,
        std::enable_if_t<!is_root_sm_conf_v<ConfType>, bool> = true
    >
    submachine(Machine& mach, decayed_parent_context_type& parent_ctx):
        simple_state_(mach, parent_ctx),
        regions_(uniform_construct, mach, context())
    {
    }

    auto& context()
    {
        return simple_state_.context();
    }

    const auto& context() const
    {
        return simple_state_.context();
    }

    auto& context_or(decayed_parent_context_type& parent_ctx)
    {
        return simple_state_.context_or(parent_ctx);
    }

    const auto& context_or(decayed_parent_context_type& parent_ctx) const
    {
        return simple_state_.context_or(parent_ctx);
    }

    template<const auto& StatePath>
    auto& context_or(decayed_parent_context_type& parent_ctx)
    {
        if constexpr(StatePath.empty())
        {
            return context_or(parent_ctx);
        }
        else
        {
            static constexpr int region_index = path_raw_head(StatePath);
            static constexpr auto state_path_tail = path_tail(StatePath);
            return tuple_get<region_index>(regions_).template context_or<state_path_tail>(context_or(parent_ctx));
        }
    }

    template<const auto& StatePath>
    const auto& context_or(decayed_parent_context_type& parent_ctx) const
    {
        if constexpr(StatePath.empty())
        {
            return context_or(parent_ctx);
        }
        else
        {
            static constexpr int region_index = path_raw_head(StatePath);
            static constexpr auto state_path_tail = path_tail(StatePath);
            return tuple_get<region_index>(regions_).template context_or<state_path_tail>(context_or(parent_ctx));
        }
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
        static_assert(tlu::size_v<transition_table_type_list> == 1);

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

    template<class Machine, class Event>
    void call_entry_action(Machine& mach, const Event& event)
    {
        simple_state_.call_entry_action(mach, event);
        tlu::for_each<region_tuple_type, region_start>(*this, mach, event);
    }

    template<bool Dry = false, class Machine, class Event>
    void call_internal_action(Machine& mach, const Event& event)
    {
        call_internal_action_2<Dry>(*this, mach, event);
    }

    template<bool Dry = false, class Machine, class Event>
    void call_internal_action
    (
        Machine& mach,
        const Event& event,
        bool& processed
    )
    {
        call_internal_action_2<Dry>(*this, mach, event, processed);
    }

    template<bool Dry = false, class Machine, class Event>
    void call_internal_action
    (
        Machine& mach,
        const Event& event
    ) const
    {
        call_internal_action_2<Dry>(*this, mach, event);
    }

    template<bool Dry = false, class Machine, class Event>
    void call_internal_action
    (
        Machine& mach,
        const Event& event,
        bool& processed
    ) const
    {
        call_internal_action_2<Dry>(*this, mach, event, processed);
    }

    template<class Machine, class Event>
    void call_exit_action(Machine& mach, const Event& event)
    {
        tlu::for_each<region_tuple_type, region_stop>(*this, mach, event);
        simple_state_.call_exit_action(mach, event);
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

    //We need a simple_state to manage context and actions
    using simple_state_type = simple_state<Conf, decayed_context_of_t<submachine>>;

    struct region_start
    {
        template<class Region, class Self, class Machine, class Event>
        static void call(Self& self, Machine& mach, const Event& event)
        {
            tuple_get<Region>(self.regions_).start(mach, self.context(), event);
        }
    };

    template<bool Dry>
    struct region_process_event
    {
        template<class Region, class Self, class Machine, class Event, class... MaybeBool>
        static void call(Self& self, Machine& mach, const Event& event, MaybeBool&... processed)
        {
            tuple_get<Region>(self.regions_).template process_event<Dry>(mach, self.context(), event, processed...);
        }
    };

    struct region_stop
    {
        template<class Region, class Self, class Machine, class Event>
        static void call(Self& self, Machine& mach, const Event& event)
        {
            tuple_get<Region>(self.regions_).stop(mach, self.context(), event);
        }
    };

    template<bool Dry, class Self, class Machine, class Event, class... MaybeBool>
    static void call_internal_action_2
    (
        Self& self,
        Machine& mach,
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
                    event
                );

                tlu::for_each<region_tuple_type, region_process_event<Dry>>(self, mach, event);
            }

            maybe_bool_util::set_to_true(processed...);
        }
        else
        {
            tlu::for_each<region_tuple_type, region_process_event<Dry>>(self, mach, event, processed...);
        }
    }

    simple_state_type simple_state_;
    region_tuple_type regions_;
};

} //namespace

#endif
