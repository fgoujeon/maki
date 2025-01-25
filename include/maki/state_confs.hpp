//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_STATE_CONFS_HPP
#define MAKI_STATE_CONFS_HPP

#include "state_conf.hpp"

namespace maki
{

/**
@brief Represents the state of any region before `machine::start()` is called
and after `machine::stop()` is called.
*/
constexpr auto stopped = state_conf{};

} //namespace

#endif
