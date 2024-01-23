//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TRANSITION_TABLE_FILTERS_HPP
#define MAKI_DETAIL_TRANSITION_TABLE_FILTERS_HPP

#include "tlu.hpp"
#include "../type_patterns.hpp"
#include "../null.hpp"
#include <type_traits>

namespace maki::detail::transition_table_filters
{

namespace by_event_detail
{
    template<const auto& Event>
    struct for_event
    {
        template<class TransitionConstant>
        struct matches_event_pattern
        {
            static constexpr auto value = matches_pattern
            (
                Event,
                TransitionConstant::value.event_pattern
            );
        };
    };
}

template<class TransitionConstantList, class Event>
using by_event_t = tlu::filter_t
<
    TransitionConstantList,
    by_event_detail::for_event<type<Event>>::template matches_event_pattern
>;

template<class TransitionConstantList>
using by_null_event_t = tlu::filter_t
<
    TransitionConstantList,
    by_event_detail::for_event<null_c>::template matches_event_pattern
>;

} //namespace

#endif
