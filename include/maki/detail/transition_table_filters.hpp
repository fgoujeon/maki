//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TRANSITION_TABLE_FILTERS_HPP
#define MAKI_DETAIL_TRANSITION_TABLE_FILTERS_HPP

#include "tlu/filter.hpp"
#include "integer_constant_sequence.hpp"
#include "../catch.hpp"
#include "../event_set.hpp"
#include "../null.hpp"

namespace maki::detail::transition_table_filters
{

template<const auto& TransitionTuple, class Event>
struct by_event_predicate_holder
{
    template<class TransitionIndexConstant>
    struct predicate
    {
        static constexpr auto make_value()
        {
            const auto& trans = tuple_get<TransitionIndexConstant::value>(TransitionTuple);
            return contained_in(event<Event>, trans.evt_set);
        }

        static constexpr auto value = make_value();
    };
};

namespace by_source_state_and_null_event_detail
{
    template<class TransitionIndexConstant, class TransitionTuple, class SourceStateId>
    constexpr auto matches(const TransitionTuple& transitions, const SourceStateId source_state_id)
    {
        const auto& trans = tuple_get<TransitionIndexConstant::value>(transitions);
        return
            equals(trans.evt_set, null) &&
            contained_in(*source_state_id, trans.source_state_builder)
        ;
    }

    template<auto TransitionTuplePtr, auto SourceStateId>
    struct predicate_holder
    {
        template<class TransitionIndexConstant>
        struct predicate
        {
            static constexpr auto value = matches<TransitionIndexConstant>(*TransitionTuplePtr, SourceStateId);
        };
    };
}

namespace by_catch_source_state_detail
{
    template<auto TransitionTuplePtr>
    struct predicate_holder
    {
        template<class TransitionIndexConstant>
        struct predicate
        {
            static constexpr auto value = equals
            (
                tuple_get<TransitionIndexConstant::value>(*TransitionTuplePtr).source_state_builder,
                &catch_
            );
        };
    };
}

template<const auto& TransitionTuple, class Event>
using by_event_t = tlu::filter_t
<
    make_integer_constant_sequence<int, TransitionTuple.size>,
    by_event_predicate_holder<TransitionTuple, Event>::template predicate
>;

template<const auto& TransitionTuple, auto SourceStateId>
using by_source_state_and_null_event_t = tlu::filter_t
<
    make_integer_constant_sequence<int, TransitionTuple.size>,
    by_source_state_and_null_event_detail::predicate_holder<&TransitionTuple, SourceStateId>::template predicate
>;

template<const auto& TransitionTuple>
using by_catch_source_state_t = tlu::filter_t
<
    make_integer_constant_sequence<int, TransitionTuple.size>,
    by_catch_source_state_detail::predicate_holder<&TransitionTuple>::template predicate
>;

} //namespace

#endif
