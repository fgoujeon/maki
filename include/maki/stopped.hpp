//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_STOPPED_HPP
#define MAKI_STOPPED_HPP

#include <type_traits>

namespace maki
{

namespace detail
{
    struct stopped_t_impl
    {
    };
}

#ifdef MAKI_DETAIL_DOXYGEN
/**
@brief The type of `maki::stopped`
*/
using stopped_t = IMPLEMENTATION_DETAIL;
#else
/*
We need an integral type so that we can directly pass `stopped` as a template
argument.
*/
using stopped_t = const detail::stopped_t_impl*;
#endif

/**
@relates stopped_t
@brief TODO
*/
#ifdef MAKI_DETAIL_DOXYGEN
constexpr auto stopped = stopped_t{};
#else
inline constexpr auto stopped = stopped_t{nullptr};
#endif

namespace detail
{
    template<class T>
    constexpr bool is_stopped_v = std::is_same_v<T, stopped_t>;
}

} //namespace

#endif
