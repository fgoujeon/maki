//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TRANSITION_TABLE_FILTERS_HPP
#define MAKI_DETAIL_TRANSITION_TABLE_FILTERS_HPP

#include "tlu.hpp"
#include "tu.hpp"
#include "../type_patterns.hpp"
#include "../null.hpp"
#include <type_traits>

namespace maki::detail::transition_table_filters
{

namespace by_event_detail
{
    template<const auto& Event>
    constexpr auto matches_event_pattern = [](const auto transition_constant)
    {
        return matches_pattern(Event, transition_constant.value.event_pattern);
    };
}

template<const auto& TransitionTuple, class Event>
constexpr auto by_event_v = tu::static_filter_v
<
    TransitionTuple,
    by_event_detail::matches_event_pattern<type<Event>>
>;

namespace by_null_event_detail
{
    template<auto SourceStateConfPtr>
    constexpr auto has_null_event = [](const auto transition_constant)
    {
        return
            transition_constant.value.psource_state_conf_pattern == static_cast<const void*>(SourceStateConfPtr) &&
            is_null(transition_constant.value.event_pattern)
        ;
    };
}

template<const auto& TransitionTuple, auto SourceStateConfPtr>
constexpr auto by_null_event_v = tu::static_filter_v
<
    TransitionTuple,
    by_null_event_detail::has_null_event<SourceStateConfPtr>
>;

} //namespace

#endif
