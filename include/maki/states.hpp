//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_STATES_HPP
#define MAKI_STATES_HPP

#include "state.hpp"
#include "state_mold.hpp"

namespace maki
{

namespace detail
{
    struct null_context{};
    struct fin_context{};
}

namespace detail::state_molds
{
    inline constexpr auto null = state_mold{}
        .context_v<null_context>()
        .pretty_name("");

    inline constexpr auto fin = state_mold{}
        .context_v<fin_context>();
}

/**
@brief The state mold of the undefined state.
*/
inline constexpr auto undefined = state_mold{};

} //namespace

#endif
