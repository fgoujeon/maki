//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SIMPLE_SM_FWD_HPP
#define AWESM_SIMPLE_SM_FWD_HPP

#include "region_configuration.hpp"
#include "sm_configuration.hpp"
#include "simple_sm_configuration.hpp"
#include "detail/type_list.hpp"

namespace awesm
{

template<class Configuration>
class sm;

namespace detail
{
    template<class Configuration>
    struct simple_sm_to_sm_configuration_helper
    {
        struct region_conf: region_configuration
        {
            using transition_table =
                typename Configuration::transition_table
            ;

            static constexpr auto enable_in_state_internal_transitions =
                Configuration::enable_in_state_internal_transitions
            ;
        };

        struct type: sm_configuration
        {
            using region_configurations = detail::type_list
            <
                region_conf
            >;

            template<class Sm>
            using exception_handler =
                typename Configuration::template exception_handler<Sm>
            ;

            template<class Sm>
            using state_transition_hook_set =
                typename Configuration::template state_transition_hook_set<Sm>
            ;
        };
    };

    template<class Configuration>
    using simple_sm_to_sm_configuration =
        typename simple_sm_to_sm_configuration_helper<Configuration>::type
    ;
}

template<class Configuration>
using simple_sm = sm<detail::simple_sm_to_sm_configuration<Configuration>>;

} //namespace

#endif
