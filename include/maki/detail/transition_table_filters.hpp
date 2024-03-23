//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TRANSITION_TABLE_FILTERS_HPP
#define MAKI_DETAIL_TRANSITION_TABLE_FILTERS_HPP

#include "tlu.hpp"
#include "integer_constant_sequence.hpp"
#include "../filters.hpp"
#include "../null.hpp"
#include <type_traits>

namespace maki::detail::transition_table_filters
{

namespace by_event_detail
{
    template<const auto& TransitionTuple, const auto& Event>
    struct operation_holder
    {
        template<int TransitionIndex>
        static constexpr auto matches = matches_filter
        (
            Event,
            tuple_get<TransitionIndex>(TransitionTuple).event_filter
        );

        template<class TransitionPtrConstantList, class TransitionIndexConstant>
        using operation = tlu::push_back_if_t
        <
            TransitionPtrConstantList,
            TransitionIndexConstant,
            matches<TransitionIndexConstant::value>
        >;
    };
}

template<const auto& TransitionTuple, class Event>
using by_event_t = tlu::left_fold_t
<
    make_integer_constant_sequence<int, TransitionTuple.size>,
    by_event_detail::operation_holder<TransitionTuple, type<Event>>::template operation,
    type_list<>
>;

template<const auto& TransitionTuple>
using by_null_event_t = tlu::left_fold_t
<
    make_integer_constant_sequence<int, TransitionTuple.size>,
    by_event_detail::operation_holder<TransitionTuple, null>::template operation,
    type_list<>
>;

} //namespace

#endif
