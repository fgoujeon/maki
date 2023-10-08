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
#include "region_path_of.hpp"
#include "machine_object_holder.hpp"
#include "context_holder.hpp"
#include "submachine_fwd.hpp"
#include "tuple.hpp"
#include "../machine_fwd.hpp"
#include "../state_conf.hpp"
#include "../transition_table.hpp"
#include "../region_path.hpp"
#include "../type_patterns.hpp"
#include <type_traits>

namespace maki::detail
{

template<class Def, class ParentRegion>
struct region_path_of<submachine<Def, ParentRegion>>
{
    static constexpr auto value = region_path_of_v<ParentRegion>;
};

template<class Def>
struct region_path_of<submachine<Def, void>>
{
    static constexpr auto value = region_path{};
};

template<class Def, class ParentRegion>
struct machine_of<submachine<Def, ParentRegion>>
{
    using type = root_sm_of_t<ParentRegion>;

    static type& get(submachine<Def, ParentRegion>& node)
    {
        return node.root_sm();
    }
};

template<class Def>
struct machine_of<submachine<Def, void>>
{
    using type = machine<Def>;

    static type& get(submachine<Def, void>& node)
    {
        return node.root_sm();
    }
};

template<class Def, class ParentRegion>
struct submachine_context
{
    /*
    Context type is either (in this order of priority):
    - the one specified in the submachine_opts::context option, if any;
    - a reference to the context type of the parent SM (not necessarily the root
      SM).
    */
    using type = std::conditional_t
    <
        Def::conf.context_type == type_c<void>,
        typename ParentRegion::parent_sm_type::context_type&,
        typename decltype(Def::conf.context_type)::type
    >;
};

template<class Def>
struct submachine_context<Def, void>
{
    using type = typename decltype(Def::conf.context_type)::type;
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

template<class Def, class ParentRegion>
class submachine
{
public:
    static constexpr auto conf = state_conf_c
        .enable_on_entry()
        .enable_on_event_for(type_list_c<maki::any>)
        .enable_on_exit()
    ;

    using def_type = Def;
    using context_type = typename submachine_context<Def, ParentRegion>::type;
    using root_sm_type = root_sm_of_t<submachine>;

    using transition_table_type_list = decltype(Def::conf.transition_tables);

    template<class... ContextArgs>
    submachine(root_sm_type& root_sm, ContextArgs&&... ctx_args):
        root_sm_(root_sm),
        ctx_holder_(root_sm, std::forward<ContextArgs>(ctx_args)...),
        def_holder_(root_sm, context()),
        regions_(*this)
    {
    }

    root_sm_type& root_sm()
    {
        return root_sm_;
    }

    context_type& context()
    {
        return ctx_holder_.get();
    }

    Def& def()
    {
        return def_holder_.get();
    }

    template<const auto& StateRegionPath, class StateDef>
    StateDef& state_def()
    {
        using state_region_path_t = std::decay_t<decltype(StateRegionPath)>;

        static_assert
        (
            std::is_same_v
            <
                typename detail::tlu::front_t<state_region_path_t>::machine_def_type,
                Def
            >
        );

        static constexpr auto region_index = tlu::front_t<state_region_path_t>::region_index;
        static constexpr auto state_region_relative_path = tlu::pop_front_t<state_region_path_t>{};
        return get<region_index>(regions_).template state_def<state_region_relative_path, StateDef>();
    }

    template<const auto& StateRegionPath, class StateDef>
    const StateDef& state_def() const
    {
        using state_region_path_t = std::decay_t<decltype(StateRegionPath)>;

        static_assert
        (
            std::is_same_v
            <
                typename detail::tlu::front_t<state_region_path_t>::machine_def_type,
                Def
            >
        );

        static constexpr auto region_index = tlu::front_t<state_region_path_t>::region_index;
        static constexpr auto state_region_relative_path = tlu::pop_front_t<state_region_path_t>{};
        return get<region_index>(regions_).template state_def<state_region_relative_path, StateDef>();
    }

    template<const auto& StateRegionPath, class StateDef>
    [[nodiscard]] bool is_active_state_def() const
    {
        using state_region_path_t = std::decay_t<decltype(StateRegionPath)>;

        static_assert
        (
            std::is_same_v
            <
                typename detail::tlu::front_t<state_region_path_t>::machine_def_type,
                Def
            >
        );

        static constexpr auto region_index = tlu::front_t<state_region_path_t>::region_index;
        static constexpr auto state_region_relative_path = tlu::pop_front_t<state_region_path_t>{};
        return get<region_index>(regions_).template is_active_state_def<state_region_relative_path, StateDef>();
    }

    template<class StateDef>
    [[nodiscard]] bool is_active_state_def() const
    {
        static_assert(tlu::size_v<transition_table_type_list> == 1);

        static constexpr auto state_region_relative_path = region_path<>{};
        return get<0>(regions_).template is_active_state_def<state_region_relative_path, StateDef>();
    }

    template<const auto& RegionPath>
    [[nodiscard]] bool is_running() const
    {
        return !is_active_state_def<RegionPath, states::stopped>();
    }

    [[nodiscard]] bool is_running() const
    {
        return !is_active_state_def<states::stopped>();
    }

    template<class Event>
    void on_entry(const Event& event)
    {
        call_on_entry(def_holder_.get(), root_sm_, event);
        tlu::for_each<region_tuple_type, region_start>(*this, event);
    }

    template<class Event>
    void on_event(const Event& event)
    {
        if constexpr(state_traits::requires_on_event_v<Def, Event>)
        {
            call_on_event(def_holder_.get(), root_sm_, context(), event);
        }

        tlu::for_each<region_tuple_type, region_process_event>(*this, event);
    }

    template<class Event>
    void on_event(const Event& event, bool& processed)
    {
        if constexpr(state_traits::requires_on_event_v<Def, Event>)
        {
            call_on_event(def_holder_.get(), root_sm_, context(), event);
            tlu::for_each<region_tuple_type, region_process_event>(*this, event);
            processed = true;
        }
        else
        {
            tlu::for_each<region_tuple_type, region_process_event>(*this, event, processed);
        }
    }

    template<class Event>
    void on_exit(const Event& event)
    {
        tlu::for_each<region_tuple_type, region_stop>(*this, event);
        call_on_exit(def_holder_.get(), root_sm_, event);
    }

private:
    using region_tuple_type = typename region_tuple
    <
        submachine,
        std::make_integer_sequence<int, tlu::size_v<transition_table_type_list>>
    >::type;

    struct region_start
    {
        template<class Region, class Event>
        static void call(submachine& self, const Event& event)
        {
            get<Region>(self.regions_).start(event);
        }
    };

    struct region_process_event
    {
        template<class Region, class Event, class... ExtraArgs>
        static void call(submachine& self, const Event& event, ExtraArgs&... extra_args)
        {
            get<Region>(self.regions_).process_event(event, extra_args...);
        }
    };

    struct region_stop
    {
        template<class Region, class Event>
        static void call(submachine& self, const Event& event)
        {
            get<Region>(self.regions_).stop(event);
        }
    };

    //Store references for faster access
    root_sm_type& root_sm_; //NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)

    context_holder<context_type> ctx_holder_;
    detail::machine_object_holder<Def> def_holder_;
    region_tuple_type regions_;
};

} //namespace

#endif
