//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_NULL_HPP
#define MAKI_NULL_HPP

namespace maki
{

/**
@brief A null event or target state.

Represents either:
- a null event (for anonymous transitions);
- a null target state (for internal transitions in transition table).
*/
struct null_t{};

inline constexpr auto null = null_t{};

inline constexpr bool operator==(const null_t /*lhs*/, const null_t /*rhs*/)
{
    return true;
}

inline constexpr bool is_null(const null_t /*ignored*/)
{
    return true;
}

template<class Other>
inline constexpr bool is_null(const Other& /*ignored*/)
{
    return false;
}

} //namespace

#endif
