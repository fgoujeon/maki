//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_PUSH_FRONT_IF_HPP
#define MAKI_DETAIL_TLU_PUSH_FRONT_IF_HPP

namespace maki::detail::tlu
{

/*
`push_front_if` adds `U` to the front of `TList` if `Condition` is `true`.
*/
template<class TList, class U, bool Condition>
struct push_front_if
{
    using type = TList;
};

template<template<class...> class TList, class... Ts, class U>
struct push_front_if<TList<Ts...>, U, true>
{
    using type = TList<U, Ts...>;
};

template<class TList, class U, bool Condition>
using push_front_if_t = typename push_front_if<TList, U, Condition>::type;

} //namespace

#endif
