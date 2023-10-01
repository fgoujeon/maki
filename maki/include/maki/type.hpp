//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_TYPE_HPP
#define MAKI_TYPE_HPP

#include <type_traits>

namespace maki
{

//A type holder
template<class T>
struct type_impl
{
    using type = T;
};

template<class T, class U>
constexpr bool operator==(const type_impl<T> /*lhs*/, const type_impl<U> /*rhs*/)
{
    return std::is_same_v<T, U>;
}

template<class T>
using type = type_impl<T>;

template<class T>
constexpr auto type_c = type<T>{};

} //namespace

#endif
