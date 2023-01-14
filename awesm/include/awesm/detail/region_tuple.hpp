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
struct region_holder_tuple;

template<class SmPath, int... RegionIndexes, class... TransitionTables>
struct region_holder_tuple<SmPath, std::integer_sequence<int, RegionIndexes...>, TransitionTables...>
{
    using type = tuple<region<make_region_path_t<SmPath, RegionIndexes>, TransitionTables>...>;
};

template<class SmPath, class RegionIndexSequence, class... TransitionTables>
using region_holder_tuple_t =
    typename region_holder_tuple<SmPath, RegionIndexSequence, TransitionTables...>::type
;

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

        template<class State, int RegionIndex>
        [[nodiscard]] bool is_active_state() const
        {
            return get<RegionIndex>(regions_).template is_active_state<State>();
        }

        template<class Event>
        void start(const Event& event)
        {
            for_each
            (
                regions_,
                [&](auto& reg)
                {
                    reg.start(event);
                }
            );
        }

        template<class Event>
        void stop(const Event& event)
        {
            for_each
            (
                regions_,
                [&](auto& reg)
                {
                    reg.stop(event);
                }
            );
        }

        template<class Event>
        void process_event(const Event& event)
        {
            for_each
            (
                regions_,
                [&](auto& reg)
                {
                    reg.process_event(event);
                }
            );
        }

    private:
        using region_indexes = std::make_integer_sequence
        <
            int,
            static_cast<int>(sizeof...(TransitionTables))
        >;
        region_holder_tuple_t<SmPath, region_indexes, TransitionTables...> regions_;
};

} //namespace

#endif
