//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SIMPLE_SUBSM_HPP
#define AWESM_SIMPLE_SUBSM_HPP

#include "subsm.hpp"
#include "region_configuration_list.hpp"

namespace awesm
{

namespace detail
{
    template<class TransitionTableHolder>
    struct simple_subsm_conf_to_subsm_conf_helper
    {
        struct region_conf_list
        {
            using type = region_configuration_list<TransitionTableHolder>;
        };
    };

    template<class TransitionTableHolder>
    using simple_subsm_conf_to_subsm_conf =
        typename simple_subsm_conf_to_subsm_conf_helper<TransitionTableHolder>::region_conf_list
    ;
}

template<class TransitionTableHolder>
using simple_subsm = subsm
<
    detail::simple_subsm_conf_to_subsm_conf<TransitionTableHolder>
>;

}

#endif
