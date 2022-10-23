//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SM_CONF_HPP
#define AWESM_SM_CONF_HPP

namespace awesm
{

template<class TransitionTableList, class... Options>
struct sm_conf
{
    using transition_table_list_t = TransitionTableList;
};

} //namespace

#endif
