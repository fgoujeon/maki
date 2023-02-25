//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_IS_BRACE_CONSTRUCTIBLE_HPP
#define AWESM_DETAIL_IS_BRACE_CONSTRUCTIBLE_HPP

#include "overload_priority.hpp"

namespace awesm::detail
{

namespace is_brace_constructible_detail
{
    template<class T, class... Args>
    constexpr auto impl(overload_priority::high /*ignored*/) -> decltype(T{std::declval<Args>()...}, bool())
    {
        return true;
    }

    template<class T, class... Args>
    constexpr bool impl(overload_priority::low /*ignored*/)
    {
        return false;
    }
}

template<class T, class... Args>
constexpr bool is_brace_constructible =
    is_brace_constructible_detail::impl<T, Args...>(overload_priority::probe)
;

} //namespace

#endif
