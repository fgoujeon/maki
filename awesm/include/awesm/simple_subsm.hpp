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
    template<class Configuration>
    struct simple_subsm_to_subsm_configuration_helper
    {
        struct region_conf: region_configuration
        {
            using transition_table =
                typename Configuration::transition_table
            ;
        };

        struct type: subsm_configuration
        {
            using region_configurations = region_configuration_list
            <
                region_conf
            >;

            using exception_handler = typename Configuration::exception_handler;

            using state_transition_hook_set =
                typename Configuration::state_transition_hook_set
            ;

            static constexpr auto enable_in_state_internal_transitions =
                Configuration::enable_in_state_internal_transitions
            ;
        };
    };

    template<class Configuration>
    using simple_subsm_to_subsm_configuration =
        typename simple_subsm_to_subsm_configuration_helper<Configuration>::type
    ;
}

template<class Configuration>
using simple_subsm = subsm<detail::simple_subsm_to_subsm_configuration<Configuration>>;

}

#endif
