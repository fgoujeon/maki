//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_NULL_HPP
#define MAKI_NULL_HPP

#include <type_traits>

namespace maki
{

#ifdef MAKI_DETAIL_DOXYGEN
/**
@brief The type of `maki::null`
*/
using null_t = IMPLEMENTATION_DETAIL;
#else
struct null_t
{
};
#endif

/**
@relates null_t
@brief A null event or target state.

Represents either:
- a null event (for completion transitions);
- a null target state (for internal transitions in transition table).

Not to be confused with `maki::states::null`.
*/
constexpr auto null = null_t{};

namespace detail
{
    template<class T>
    constexpr bool is_null_v = std::is_same_v<T, null_t>;
}

} //namespace

#endif
