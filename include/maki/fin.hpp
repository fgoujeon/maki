//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_FIN_HPP
#define MAKI_FIN_HPP

#include <type_traits>

namespace maki
{

#ifdef MAKI_DETAIL_DOXYGEN
/**
@brief The type of `maki::fin`
*/
using fin_t = IMPLEMENTATION_DETAIL;
#else
struct fin_t{};
#endif

/**
@relates fin_t
@brief Represents the final state in transition tables, to be passed as a target
state to `maki::transition_table::operator()()`.
*/
inline constexpr auto fin = fin_t{};

namespace detail
{
    template<class T>
    constexpr bool is_fin_v = std::is_same_v<T, fin_t>;
}

} //namespace

#endif
