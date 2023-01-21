//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_STATES_HPP
#define AWESM_DETAIL_STATES_HPP

#include "state_conf.hpp"

namespace awesm::states
{

/*
The initial state of any region, i.e. the state before start() and after stop().
*/
struct stopped
{
    using conf_type = state_conf<>;
};

} //namespace

#endif
