//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_TYPE_LIST_FILTERS_HPP
#define MAKI_DETAIL_STATE_TYPE_LIST_FILTERS_HPP

#include "tlu/filter.hpp"
#include "../filters.hpp"

namespace maki::detail::state_type_list_filters
{

namespace by_filter_detail
{
    template<auto FilterPtr>
    struct for_filter
    {
        template<class StateIdConstant>
        struct matches
        {
            static constexpr auto value = matches_filter(StateIdConstant::value, *FilterPtr);
        };
    };
}

template<class StateIdConstantList, auto FilterPtr>
using by_filter_t = tlu::filter_t
<
    StateIdConstantList,
    by_filter_detail::for_filter<FilterPtr>::template matches
>;

namespace by_required_on_event_detail
{
    template<class Region, class Event>
    struct with
    {
        template<class State>
        struct requires_on_event
        {
            static constexpr auto value = State::template has_internal_action_for_event<Event>();
        };
    };
}

template<class StateList, class Region, class Event>
using by_required_on_event_t = tlu::filter_t
<
    StateList,
    by_required_on_event_detail::with<Region, Event>::template requires_on_event
>;

} //namespace

#endif
