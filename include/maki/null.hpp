//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_NULL_HPP
#define MAKI_NULL_HPP

#include <type_traits>

namespace maki
{

namespace detail
{
    struct null_t_impl
    {
    };
}

#ifdef MAKI_DETAIL_DOXYGEN
/**
@brief The type of `maki::null`
*/
using null_t = IMPLEMENTATION_DETAIL;
#else
/*
We need an integral type so that we can directly pass `null` as a template
argument.
*/
using null_t = const detail::null_t_impl*;
#endif

/**
@relates null_t
@brief A null event or target state.

Represents either:
- a null event (for completion transitions);
- a null target state (for internal transitions in transition table).

Not to be confused with `maki::states::null`.
*/
#ifdef MAKI_DETAIL_DOXYGEN
constexpr auto null = null_t{};
#else
inline constexpr auto null = null_t{nullptr};
#endif

namespace detail
{
    template<class T>
    constexpr bool is_null_v = std::is_same_v<T, null_t>;
}

} //namespace

#endif
