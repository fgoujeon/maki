//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_STOPPED_HPP
#define MAKI_STOPPED_HPP

#include "state_conf.hpp"

namespace maki
{

namespace detail
{
    struct stopped_t{};
}

/**
@brief To be given as a target state of a transition to stop the machine.
*/
#ifdef MAKI_DETAIL_DOXYGEN
constexpr auto stopped = IMPLEMENTATION_DETAIL;
#else
constexpr auto stopped = detail::stopped_t{};
#endif

namespace detail
{
    template<class T>
    constexpr bool is_stopped_v = std::is_same_v<T, stopped_t>;
}

} //namespace

#endif
