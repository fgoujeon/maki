// Copyright Florian Goujeon 2021 - 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
// Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TRANSITION_TABLE_PREDICATES_HPP
#define MAKI_DETAIL_TRANSITION_TABLE_PREDICATES_HPP

#include "../transition_table.hpp"
#include "friendly_impl.hpp"
#include "machine_conf_tree.hpp"
#include "tlu/get.hpp"
#include "tuple.hpp"

namespace maki::detail::transition_table_predicates
{

/*
`has_event`
*/

template<class MachineConfHolder, class TransitionTableIpath, class Event>
struct has_event
{
    template<int TransitionIndex>
    struct predicate
    {
        using trans_table_type = machine_conf_tree::
            node_at_path_t<MachineConfHolder, TransitionTableIpath>;

        using trans_type =
            tlu::get_t<impl_of_t<trans_table_type>, TransitionIndex>;

        using trans_event_type_set_type =
            transition_event_type_set_t<trans_type>;

        static constexpr bool value =
            type_set_contains_v<trans_event_type_set_type, Event>;
    };
};


/*
`has_source_state_and_null_event`
*/

namespace has_source_state_and_null_event_detail
{
    template<int TransitionIndex, class TransitionTable, class SourceStateMold>
    constexpr bool matches(
        const TransitionTable& table,
        const SourceStateMold& source_state_mold)
    {
        const auto& trans = tuple_get<TransitionIndex>(impl_of(table));
        return trans.can_process_completion_event() &&
            contained_in(source_state_mold, trans.source_state_mold);
    }
} // namespace has_source_state_and_null_event_detail

template<
    class MachineConfHolder,
    class TransitionTableIpath,
    const auto& SourceStateMold>
struct has_source_state_and_null_event
{
    template<int TransitionIndex>
    struct predicate
    {
        static constexpr bool value =
            has_source_state_and_null_event_detail::matches<TransitionIndex>(
                machine_conf_tree::
                    node_at_path_v<MachineConfHolder, TransitionTableIpath>,
                SourceStateMold);
    };
};

} // namespace maki::detail::transition_table_predicates

#endif
