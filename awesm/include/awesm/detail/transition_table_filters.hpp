//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_TRANSITION_TABLE_FILTERS_HPP
#define AWESM_DETAIL_TRANSITION_TABLE_FILTERS_HPP

#include "tlu.hpp"
#include "../type_patterns.hpp"
#include <type_traits>

namespace awesm::detail::transition_table_filters
{

namespace by_event_detail
{
    template<class Event>
    struct for_event
    {
        template<class Row>
        struct matches_event_pattern
        {
            static constexpr auto value = matches_pattern_v<Event, typename Row::event_type_pattern>;
        };
    };
}

template<class TransitionTable, class Event>
using by_event_t = tlu::filter_t
<
    TransitionTable,
    by_event_detail::for_event<Event>::template matches_event_pattern
>;

} //namespace

#endif
