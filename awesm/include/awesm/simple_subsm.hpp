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
    template<class TransitionTable>
    struct simple_subsm_conf_to_subsm_conf_helper
    {
        struct region_conf
        {
            using transition_table = TransitionTable;
        };

        struct type
        {
            using region_configurations = region_configuration_list<region_conf>;
        };
    };

    template<class TransitionTable>
    using simple_subsm_conf_to_subsm_conf =
        typename simple_subsm_conf_to_subsm_conf_helper<TransitionTable>::type
    ;
}

template<class MainConfiguration, class... Options>
using simple_subsm = subsm
<
    detail::simple_subsm_conf_to_subsm_conf
    <
        typename MainConfiguration::transition_table
    >,
    Options...
>;

}

#endif
