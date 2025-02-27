//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_INIT_HPP
#define MAKI_INIT_HPP

#include <type_traits>

namespace maki
{

#ifdef MAKI_DETAIL_DOXYGEN
/**
@brief The type of `maki::init`
*/
using init_t = IMPLEMENTATION_DETAIL;
#else
struct init_t{};
#endif

/**
@relates init_t
@brief The initial pseudostate, to be passed as a source state to `maki::transition_table::operator()()`.
*/
inline constexpr auto init = init_t{};

namespace detail
{
    template<class T>
    constexpr bool is_init_v = std::is_same_v<T, init_t>;
}

} //namespace

#endif
