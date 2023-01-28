//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_REGION_TUPLE_HPP
#define AWESM_DETAIL_REGION_TUPLE_HPP

#include "region.hpp"
#include "tuple.hpp"
#include "../transition_table_list.hpp"
#include <type_traits>

namespace awesm::detail
{

template<class SmPath, class RegionIndexSequence, class... TransitionTables>
struct tt_list_to_region_tuple;

template<class SmPath, int... RegionIndexes, class... TransitionTables>
struct tt_list_to_region_tuple<SmPath, std::integer_sequence<int, RegionIndexes...>, TransitionTables...>
{
    using type = tuple<region<make_region_path_t<SmPath, RegionIndexes>, TransitionTables>...>;
};

template<class SmPath, class... TransitionTables>
using tt_list_to_region_tuple_t = typename tt_list_to_region_tuple
<
    SmPath,
    std::make_integer_sequence
    <
        int,
        static_cast<int>(sizeof...(TransitionTables))
    >,
    TransitionTables...
>::type;

template<class SmPath, class TransitionTableList>
class region_tuple;

template<class SmPath, class... TransitionTables>
class region_tuple<SmPath, transition_table_list<TransitionTables...>>
{
    public:
        using sm_type = sm_path_to_sm_t<SmPath>;

        explicit region_tuple(sm_type& mach):
            regions_(mach)
        {
        }

        region_tuple(const region_tuple&) = delete;
        region_tuple(region_tuple&&) = delete;
        region_tuple& operator=(const region_tuple&) = delete;
        region_tuple& operator=(region_tuple&&) = delete;
        ~region_tuple() = default;

        template<int RegionIndex>
        const auto& get_region() const
        {
            return get<RegionIndex>(regions_);
        }

        template<class State, int RegionIndex>
        const auto& get_state() const
        {
            return get_region<RegionIndex>().template get_state<State>();
        }

        template<class StateRegionPath, class State>
        [[nodiscard]] bool is_active_state() const
        {
            static constexpr auto region_index = region_path_front_element_t<StateRegionPath>::region_index;
            return get<region_index>(regions_).template is_active_state<tlu::pop_front_t<StateRegionPath>, State>();
        }

        template<class State>
        [[nodiscard]] bool is_active_state() const
        {
            static_assert(sizeof...(TransitionTables) == 1);
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
            SmPath,
            TransitionTables...
        >;
        region_tuple_type regions_;
};

} //namespace

#endif
