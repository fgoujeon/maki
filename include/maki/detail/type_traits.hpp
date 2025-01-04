//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TYPE_TRAITS_HPP
#define MAKI_DETAIL_TYPE_TRAITS_HPP

#include "overload_priority.hpp"
#include <utility>

namespace maki::detail
{

//
//is_nullary
//

template<class F>
constexpr bool is_nullary_impl(overload_priority::low /*ignored*/)
{
    return false;
}

template<class F>
constexpr auto is_nullary_impl(overload_priority::high /*ignored*/) -> decltype(std::declval<F>()(), bool())
{
    return true;
}

template<class F>
struct is_nullary
{
    static constexpr auto value = is_nullary_impl<F>(overload_priority::probe);
};

template<class F>
constexpr auto is_nullary_v = is_nullary<F>::value;

} //namespace

#endif
