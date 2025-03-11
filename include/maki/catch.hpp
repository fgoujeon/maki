//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_CATCH_HPP
#define MAKI_CATCH_HPP

#include <type_traits>

namespace maki
{

#ifdef MAKI_DETAIL_DOXYGEN
/**
@brief The type of `maki::catch_`
*/
using catch_t = IMPLEMENTATION_DETAIL;
#else
struct catch_t{};
#endif

/**
@relates catch_t
@brief The catch pseudostate, to be passed as a source state to `maki::transition_table::operator()()`.
*/
inline constexpr auto catch_ = catch_t{};

namespace detail
{
    template<class T>
    constexpr bool is_catch_v = std::is_same_v<T, catch_t>;
}

} //namespace

#endif
