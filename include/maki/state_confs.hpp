//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::state_confs namespace and its types
*/

#ifndef MAKI_DETAIL_STATE_CONFS_HPP
#define MAKI_DETAIL_STATE_CONFS_HPP

#include "state_conf.hpp"

/**
@brief Some predefined state confs used by Maki itself
*/
namespace maki::state_confs
{

/**
@brief Represents the state of any region before @ref machine::start() is called and
after @ref machine::stop() is called.
*/
constexpr auto stopped = state_conf{};

} //namespace

#endif
