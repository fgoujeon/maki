//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_POP_FRONT_HPP
#define MAKI_DETAIL_TLU_POP_FRONT_HPP

namespace maki::detail::tlu
{

/*
pop_front removes the first type of a typelist.

In this example...:
    using typelist = tuple<char, short, int>;
    using typelist2 = pop_front_t<typelist>;

... typelist2 is an alias of tuple<short, int>.
*/
template<class TListT>
struct pop_front;

template<template<class...> class TList, class T, class... Ts>
struct pop_front<TList<T, Ts...>>
{
    using type = TList<Ts...>;
};

template<class TListT>
using pop_front_t = typename pop_front<TListT>::type;

} //namespace

#endif
