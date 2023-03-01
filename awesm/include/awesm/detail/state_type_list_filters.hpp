//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_STATE_TYPE_LIST_FILTERS_HPP
#define AWESM_DETAIL_STATE_TYPE_LIST_FILTERS_HPP

#include "state_traits.hpp"
#include "tlu.hpp"
#include "../type_filters.hpp"
#include <type_traits>

namespace awesm::detail::state_type_list_filters
{

namespace by_filter_detail
{
    template<class Filter>
    struct for_filter
    {
        template<class State>
        struct matches
        {
            static constexpr auto value = matches_filter_v<State, Filter>;
        };
    };
}

template<class StateList, class Filter>
using by_filter_t = tlu::filter_t
<
    StateList,
    by_filter_detail::for_filter<Filter>::template matches
>;

namespace by_required_on_event_detail
{
    template<class Region, class Event>
    struct with
    {
        template<class State>
        struct requires_on_event
        {
            static constexpr auto value = state_traits::requires_on_event_v
            <
                state_traits::wrap_t<State, Region>,
                Event
            >;
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
