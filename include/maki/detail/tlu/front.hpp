//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_FRONT_HPP
#define MAKI_DETAIL_TLU_FRONT_HPP

namespace maki::detail::tlu
{

template<class TList>
struct front;

template<template<class...> class TList, class T, class... Ts>
struct front<TList<T, Ts...>>
{
    using type = T;
};

template<class TList>
using front_t = typename front<TList>::type;

} //namespace

#endif
