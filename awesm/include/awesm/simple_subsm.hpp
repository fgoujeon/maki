//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SIMPLE_SUBSM_HPP
#define AWESM_SIMPLE_SUBSM_HPP

#include "subsm.hpp"
#include "region_list.hpp"

namespace awesm
{

namespace detail
{
    template<class TransitionTable>
    struct tt_to_region_list_holder
    {
        using type = region_list<region<TransitionTable>>;
    };
}

template<class TransitionTableHolder>
using simple_subsm = subsm
<
    detail::tt_to_region_list_holder<typename TransitionTableHolder::type>
>;

}

#endif
