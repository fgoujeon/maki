//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATES_HPP
#define MAKI_DETAIL_STATES_HPP

#include "state_conf.hpp"

namespace maki::states
{

/**
@brief Represents the state of any region before @ref machine::start() is called and
after @ref machine::stop() is called.
*/
struct stopped
{
    using conf = state_conf_tpl<>;
};

} //namespace

#endif
