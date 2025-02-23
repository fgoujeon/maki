//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATES_HPP
#define MAKI_DETAIL_STATES_HPP

#include "state_impls/simple_no_context.hpp"
#include "../state.hpp"
#include "../state_conf.hpp"

namespace maki::detail
{

namespace state_confs
{
    inline constexpr auto initial = state_conf{};
    inline constexpr auto final = state_conf{};
}

namespace states
{
    inline constexpr auto initial = state<state_impls::simple_no_context<&state_confs::initial>>{};
    inline constexpr auto final = state<state_impls::simple_no_context<&state_confs::final>>{};
}

} //namespace

#endif
