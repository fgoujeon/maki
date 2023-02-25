//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_SUBSM_HPP
#define AWESM_DETAIL_SUBSM_HPP

#include "alternative.hpp"
#include "call_member.hpp"
#include "clu.hpp"
#include "tlu.hpp"
#include "region.hpp"
#include "region_path_of.hpp"
#include "sm_conf_traits.hpp"
#include "sm_object_holder.hpp"
#include "subsm_fwd.hpp"
#include "tuple.hpp"
#include "../state_conf.hpp"
#include "../transition_table.hpp"
#include "../region_path.hpp"
#include <type_traits>

namespace awesm::detail
{

template<class Def, class ParentRegion, bool Root>
struct region_path_of<subsm<Def, ParentRegion, Root>>
{
    using type = region_path_of_t<ParentRegion>;
};

template<class Def, class ParentRegion, bool Root>
struct root_sm_of<subsm<Def, ParentRegion, Root>>
{
    using type = root_sm_of_t<ParentRegion>;
};

template
<
    class ParentSm,
    class TransitionTableFnList,
    class RegionIndexSequence
>
struct tt_list_to_region_tuple;

template
<
    class ParentSm,
    template<auto...> class TransitionTableFnList,
    auto... TransitionTableFns,
    int... RegionIndexes
>
struct tt_list_to_region_tuple
<
    ParentSm,
    TransitionTableFnList<TransitionTableFns...>,
    std::integer_sequence<int, RegionIndexes...>
>
{
    using type = tuple
    <
        region
        <
            ParentSm,
            RegionIndexes,
            TransitionTableFns
        >...
    >;
};

template<class ParentSm, class TransitionTableFnList>
using tt_list_to_region_tuple_t = typename tt_list_to_region_tuple
<
    ParentSm,
    TransitionTableFnList,
    std::make_integer_sequence<int, clu::size_v<TransitionTableFnList>>
>::type;

template<class Def, class ParentRegion, bool Root>
class subsm
{
    public:
        using root_sm_type = root_sm_of_t<subsm>;
        using subsm_conf_type = typename Def::conf;
        using parent_sm_context_type = typename ParentRegion::parent_sm_type::context_type;

        /*
        Context type is either (in this order of priority):
        - the one specified in the subsm_opts::context option, if any;
        - the context type of the parent SM (not necessarily the root SM).
        */
        using context_type = sm_conf_traits::context_t
        <
            subsm_conf_type,
            parent_sm_context_type&
        >;

        using conf = state_conf
        <
            state_opts::on_entry_any,
            state_opts::on_event_any,
            state_opts::on_exit_any,
            state_opts::get_pretty_name
        >;

        subsm(root_sm_type& root_sm, parent_sm_context_type& parent_ctx):
            root_sm_(root_sm),
            context_(parent_ctx),
            def_holder_(root_sm, context_),
            regions_(get_region_parent_sm())
        {
        }

        root_sm_type& get_root_sm()
        {
            return root_sm_;
        }

        auto& get_context()
        {
            return context_;
        }

        Def& get_def()
        {
            return def_holder_.get();
        }

        template<class StateRegionPath, class State>
        [[nodiscard]] bool is_active_state() const
        {
            static constexpr auto region_index = tlu::front_t<StateRegionPath>::region_index;
            return get<region_index>(regions_).template is_active_state<tlu::pop_front_t<StateRegionPath>, State>();
        }

        template<class State>
        [[nodiscard]] bool is_active_state() const
        {
            static_assert(clu::size_v<transition_table_fn_list_type> == 1);
            return get<0>(regions_).template is_active_state<region_path<>, State>();
        }

        [[nodiscard]] bool is_running() const
        {
            return !is_active_state<states::stopped>();
        }

        template<class Event>
        void on_entry(const Event& event)
        {
            call_on_entry(def_holder_.get(), root_sm_, event);

            for_each_region
            (
                [](auto& reg, const Event& event)
                {
                    reg.start(event);
                },
                event
            );
        }

        template<class Event>
        void on_event(const Event& event)
        {
            call_on_event(def_holder_.get(), event);

            for_each_region
            (
                [](auto& reg, const Event& event)
                {
                    reg.process_event(event);
                },
                event
            );
        }

        template<class Event>
        void on_exit(const Event& event)
        {
            for_each_region
            (
                [](auto& reg, const Event& event)
                {
                    reg.stop(event);
                },
                event
            );

            call_on_exit(def_holder_.get(), root_sm_, event);
        }

        static decltype(auto) get_pretty_name()
        {
            return awesm::get_pretty_name<Def>();
        }

    private:
        using transition_table_fn_list_type = sm_conf_traits::transition_table_fn_list_t<subsm_conf_type>;

        using region_parent_sm_type = alternative_t
        <
            Root,
            root_sm_type,
            subsm
        >;

        using region_tuple_type = tt_list_to_region_tuple_t
        <
            region_parent_sm_type,
            transition_table_fn_list_type
        >;

        region_parent_sm_type& get_region_parent_sm()
        {
            if constexpr(Root)
            {
                return root_sm_;
            }
            else
            {
                return *this;
            }
        }

        template<class F, class Event>
        void for_each_region(F&& fun, const Event& event)
        {
            tlu::apply_t<region_tuple_type, for_each_region_helper>::call
            (
                *this,
                std::forward<F>(fun),
                event
            );
        }

        template<class... Regions>
        struct for_each_region_helper
        {
            template<class F, class Event>
            static void call(subsm& self, F&& fun, const Event& event)
            {
                (
                    fun(get<Regions>(self.regions_), event),
                    ...
                );
            }
        };

        root_sm_type& root_sm_;
        context_type context_;
        detail::sm_object_holder<Def> def_holder_;
        region_tuple_type regions_;
};

} //namespace

#endif
