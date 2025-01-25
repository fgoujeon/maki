//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATES_HPP
#define MAKI_DETAIL_STATES_HPP

#include "simple_state_no_context.hpp"
#include "../state_confs.hpp"
#include "../state.hpp"

namespace maki::detail::states
{

inline constexpr auto stopped = state<simple_state_no_context<&maki::stopped>>{};

} //namespace

#endif
