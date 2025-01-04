//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_NULL_HPP
#define MAKI_NULL_HPP

namespace maki
{

namespace detail
{
    struct null_t_impl
    {
    };

    using null_t = const null_t_impl*;
}

/**
@brief A null event or target state.

Represents either:
- a null event (for anonymous transitions);
- a null target state (for internal transitions in transition table).
*/
#ifdef MAKI_DETAIL_DOXYGEN
constexpr auto null = IMPLEMENTATION_DETAIL;
#else
inline constexpr auto null = detail::null_t{nullptr};
#endif

} //namespace

#endif
