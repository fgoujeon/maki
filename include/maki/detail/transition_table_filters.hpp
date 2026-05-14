//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TRANSITION_TABLE_FILTERS_HPP
#define MAKI_DETAIL_TRANSITION_TABLE_FILTERS_HPP

#include "machine_conf_tree.hpp"
#include "iseq.hpp"
#include "tuple.hpp"
#include "friendly_impl.hpp"
#include "../transition_table.hpp"
#include <type_traits>

namespace maki::detail::transition_table_filters
{

/*
`by_event_t`
*/

template<class MachineConfHolder, class TransitionTableIpath, class Event>
struct by_event_predicate_holder
{
    template<int TransitionIndex>
    struct predicate
    {
        static constexpr bool make_value()
        {
            constexpr const auto& trans_table =
                machine_conf_tree::node_at_path_v
                <
                    MachineConfHolder,
                    TransitionTableIpath
                >
            ;

            constexpr const auto& trans =
                tuple_get<TransitionIndex>(impl_of(trans_table))
            ;

            using trans_t = std::decay_t<decltype(trans)>;

            using trans_event_type_set_t = transition_event_type_set_t<trans_t>;

            return type_set_contains_v<trans_event_type_set_t, Event>;
        }

        static constexpr bool value = make_value();
    };
};

template<class MachineConfHolder, class TransitionTableIpath, class Event>
using by_event_t = iseq_filter_t
<
    linear_iseq_t
    <
        impl_of
        (
            machine_conf_tree::node_at_path_v
            <
                MachineConfHolder,
                TransitionTableIpath
            >
        ).size
    >,
    by_event_predicate_holder
    <
        MachineConfHolder,
        TransitionTableIpath,
        Event
    >::template predicate
>;


/*
`by_source_state_and_null_event_t`
*/

namespace by_source_state_and_null_event_detail
{
    template<int TransitionIndex, class TransitionTable, class SourceStateMold>
    constexpr bool matches(const TransitionTable& table, const SourceStateMold& source_state_mold)
    {
        const auto& trans = tuple_get<TransitionIndex>(impl_of(table));
        return
            trans.can_process_completion_event() &&
            contained_in(source_state_mold, trans.source_state_mold)
        ;
    }

    template<class MachineConfHolder, class TransitionTableIpath, const auto& SourceStateMold>
    struct predicate_holder
    {
        template<int TransitionIndex>
        struct predicate
        {
            static constexpr bool value = matches<TransitionIndex>
            (
                machine_conf_tree::node_at_path_v
                <
                    MachineConfHolder,
                    TransitionTableIpath
                >,
                SourceStateMold
            );
        };
    };
}

template<class MachineConfHolder, class TransitionTableIpath, const auto& SourceStateMold>
using by_source_state_and_null_event_t = iseq_filter_t
<
    linear_iseq_t
    <
        impl_of
        (
            machine_conf_tree::node_at_path_v
            <
                MachineConfHolder,
                TransitionTableIpath
            >
        ).size
    >,
    by_source_state_and_null_event_detail::predicate_holder
    <
        MachineConfHolder,
        TransitionTableIpath,
        SourceStateMold
    >::template predicate
>;

} //namespace

#endif
