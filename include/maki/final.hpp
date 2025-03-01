//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_FINAL_HPP
#define MAKI_FINAL_HPP

#include <type_traits>

namespace maki
{

#ifdef MAKI_DETAIL_DOXYGEN
/**
@brief The type of `maki::final`
*/
using final_t = IMPLEMENTATION_DETAIL;
#else
struct final_t{};
#endif

/**
@relates final_t
@brief Represents the final state in transition tables, to be passed as a target
state to `maki::transition_table::operator()()`.
*/
inline constexpr auto final = final_t{};

namespace detail
{
    template<class T>
    constexpr bool is_final_v = std::is_same_v<T, final_t>;
}

} //namespace

#endif
