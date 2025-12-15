//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_TYPE_LIST_FILTERS_HPP
#define MAKI_DETAIL_STATE_TYPE_LIST_FILTERS_HPP

#include "friendly_impl.hpp"
#include "tlu/filter.hpp"

namespace maki::detail::state_type_list_filters
{

namespace by_state_set_detail
{
    template<auto StateSetPtr>
    struct for_state_set
    {
        template<class StateIdConstant>
        struct matches
        {
            static constexpr bool value = contains(impl_of(*StateSetPtr), StateIdConstant::value);
        };
    };
}

template<class StateIdConstantList, auto StateSetPtr>
using by_state_set_t = boost::mp11::mp_filter
<
    by_state_set_detail::for_state_set<StateSetPtr>::template matches,
    StateIdConstantList
>;

} //namespace

#endif
