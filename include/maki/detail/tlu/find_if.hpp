//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_FIND_IF_HPP
#define MAKI_DETAIL_TLU_FIND_IF_HPP

#include <type_traits>

namespace maki::detail::tlu
{

template<class TList, template<class> class Predicate, class Enable = void>
struct find_if_impl;

template<template<class...> class TList, class T, class... Ts, template<class> class Predicate>
struct find_if_impl<TList<T, Ts...>, Predicate, std::enable_if_t<Predicate<T>::value>>
{
    using type = T;
};

template<template<class...> class TList, class T, class... Ts, template<class> class Predicate>
struct find_if_impl<TList<T, Ts...>, Predicate, std::enable_if_t<!Predicate<T>::value>>
{
    using type = typename find_if_impl<TList<Ts...>, Predicate>::type;
};

template<class TList, template<class> class Predicate>
using find_if_t = typename find_if_impl<TList, Predicate>::type;

} //namespace

#endif
