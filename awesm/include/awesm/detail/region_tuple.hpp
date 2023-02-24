//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_REGION_TUPLE_HPP
#define AWESM_DETAIL_REGION_TUPLE_HPP

#include "../transition_table.hpp"
#include "region.hpp"
#include "tuple.hpp"
#include "clu.hpp"
#include <type_traits>

namespace awesm::detail
{

template
<
    class RootSm,
    class ParentSm,
    class Context,
    class TransitionTableFnList,
    class RegionIndexSequence
>
struct tt_list_to_region_tuple;

template
<
    class RootSm,
    class ParentSm,
    class Context,
    template<auto...> class TransitionTableFnList,
    auto... TransitionTableFns,
    int... RegionIndexes
>
struct tt_list_to_region_tuple
<
    RootSm,
    ParentSm,
    Context,
    TransitionTableFnList<TransitionTableFns...>,
    std::integer_sequence<int, RegionIndexes...>
>
{
    using type = tuple
    <
        region
        <
            RootSm,
            ParentSm,
            RegionIndexes,
            Context,
            TransitionTableFns
        >...
    >;
};

template<class RootSm, class ParentSm, class Context, class TransitionTableFnList>
using tt_list_to_region_tuple_t = typename tt_list_to_region_tuple
<
    RootSm,
    ParentSm,
    Context,
    TransitionTableFnList,
    std::make_integer_sequence<int, clu::size_v<TransitionTableFnList>>
>::type;

template<class RootSm, class ParentSm, class Context, class TransitionTableFnList>
class region_tuple
{
    public:
        region_tuple(RootSm& mach, Context& ctx):
            regions_(mach, ctx)
        {
        }

        region_tuple(const region_tuple&) = delete;
        region_tuple(region_tuple&&) = delete;
        region_tuple& operator=(const region_tuple&) = delete;
        region_tuple& operator=(region_tuple&&) = delete;
        ~region_tuple() = default;

        template<class StateRegionPath, class State>
        [[nodiscard]] bool is_active_state() const
        {
            static constexpr auto region_index = tlu::front_t<StateRegionPath>::region_index;
            return get<region_index>(regions_).template is_active_state<tlu::pop_front_t<StateRegionPath>, State>();
        }

        template<class State>
        [[nodiscard]] bool is_active_state() const
        {
            static_assert(clu::size_v<TransitionTableFnList> == 1);
            return get<0>(regions_).template is_active_state<region_path<>, State>();
        }

        template<class Event>
        void start(const Event& event)
        {
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
        void stop(const Event& event)
        {
            for_each_region
            (
                [](auto& reg, const Event& event)
                {
                    reg.stop(event);
                },
                event
            );
        }

        template<class Event>
        void process_event(const Event& event)
        {
            for_each_region
            (
                [](auto& reg, const Event& event)
                {
                    reg.process_event(event);
                },
                event
            );
        }

    private:
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
            static void call(region_tuple& self, F&& fun, const Event& event)
            {
                (
                    fun(get<Regions>(self.regions_), event),
                    ...
                );
            }
        };

        using region_tuple_type = tt_list_to_region_tuple_t
        <
            RootSm,
            ParentSm,
            Context,
            TransitionTableFnList
        >;
        region_tuple_type regions_;
};

} //namespace

#endif
