//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_EMPTY_HPP
#define MAKI_DETAIL_TLU_EMPTY_HPP

namespace maki::detail::tlu
{

template<class TList>
struct empty;

template<template<class...> class TList, class T, class... Ts>
struct empty<TList<T, Ts...>>
{
    static constexpr auto value = false;
};

template<template<class...> class TList>
struct empty<TList<>>
{
    static constexpr auto value = true;
};

template<class TList>
constexpr int empty_v = empty<TList>::value;

} //namespace

#endif
