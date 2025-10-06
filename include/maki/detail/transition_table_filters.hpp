//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TRANSITION_TABLE_FILTERS_HPP
#define MAKI_DETAIL_TRANSITION_TABLE_FILTERS_HPP

#include "tlu/filter.hpp"
#include "tuple.hpp"
#include "integer_constant_sequence.hpp"

namespace maki::detail::transition_table_filters
{

template<const auto& TransitionTable, class Event>
struct by_event_predicate_holder
{
    template<class TransitionIndexConstant>
    struct predicate
    {
        static constexpr bool make_value()
        {
            const auto& trans = tuple_get<TransitionIndexConstant::value>(impl_of(TransitionTable));
            return event_types(trans).template contains<Event>();
        }

        static constexpr bool value = make_value();
    };
};

namespace by_source_state_and_null_event_detail
{
    template<class TransitionIndexConstant, class TransitionTable, class SourceStateId>
    constexpr bool matches(const TransitionTable& table, const SourceStateId source_state_id)
    {
        const auto& trans = tuple_get<TransitionIndexConstant::value>(impl_of(table));
        return
            trans.can_process_completion_event() &&
            contained_in(*source_state_id, trans.source_state_mold)
        ;
    }

    template<auto TransitionTablePtr, auto SourceStateId>
    struct predicate_holder
    {
        template<class TransitionIndexConstant>
        struct predicate
        {
            static constexpr bool value = matches<TransitionIndexConstant>(*TransitionTablePtr, SourceStateId);
        };
    };
}

template<const auto& TransitionTable, class Event>
using by_event_t = tlu::filter_t
<
    make_integer_constant_sequence<int, impl_of(TransitionTable).size>,
    by_event_predicate_holder<TransitionTable, Event>::template predicate
>;

template<const auto& TransitionTable, auto SourceStateId>
using by_source_state_and_null_event_t = tlu::filter_t
<
    make_integer_constant_sequence<int, impl_of(TransitionTable).size>,
    by_source_state_and_null_event_detail::predicate_holder<&TransitionTable, SourceStateId>::template predicate
>;

} //namespace

#endif
