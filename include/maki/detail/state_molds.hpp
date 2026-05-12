//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_MOLDS_HPP
#define MAKI_DETAIL_STATE_MOLDS_HPP

#include "../state_mold.hpp"

namespace maki::detail::state_molds
{

inline constexpr auto null = state_mold{}
    .pretty_name("");

inline constexpr auto fin = state_mold{};

} //namespace

#endif
