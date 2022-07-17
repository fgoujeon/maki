//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SIMPLE_SM_FWD_HPP
#define AWESM_SIMPLE_SM_FWD_HPP

#include "sm_fwd.hpp"
#include "region_list.hpp"

namespace awesm
{

template<class TransitionTableHolder>
class region;

namespace detail
{
    template<class TransitionTableHolder>
    struct tt_table_to_region_list_holder
    {
        using type = region_list
        <
            region<typename TransitionTableHolder::type>
        >;
    };
}

template<class TransitionTableHolder, class... Options>
using simple_sm = sm
<
    detail::tt_table_to_region_list_holder<TransitionTableHolder>,
    Options...
>;

} //namespace

#endif
