//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_COMPOSITE_STATE_CONF_HPP
#define AWESM_COMPOSITE_STATE_CONF_HPP

#include "transition_table_list.hpp"
#include "pretty_name.hpp"

namespace awesm
{

namespace composite_state_options
{
    using get_pretty_name = detail::get_pretty_name_option;
}

template<class TransitionTable, class... Options>
struct composite_state_conf
{
    using transition_table_list_type = transition_table_list<TransitionTable>;
    static constexpr auto region_count = 1;
};

template<class... TransitionTables, class... Options>
struct composite_state_conf<transition_table_list<TransitionTables...>, Options...>
{
    using transition_table_list_type = transition_table_list<TransitionTables...>;
    static constexpr auto region_count = sizeof...(TransitionTables);
};

} //namespace

#endif
