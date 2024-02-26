//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_NULL_HPP
#define MAKI_NULL_HPP

namespace maki
{

struct null_t_impl
{
};

using null_t = const null_t_impl*;

/**
@brief A null event or target state.

Represents either:
- a null event (for anonymous transitions);
- a null target state (for internal transitions in transition table).
*/
inline constexpr auto null = null_t{nullptr};

} //namespace

#endif
