//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_APPLY_HPP
#define MAKI_DETAIL_TLU_APPLY_HPP

namespace maki::detail::tlu
{

/*
apply_t<TList, Target> is an alias of Target<Ts...>, where Ts are the types of
TList.
*/

template<class TList, template<class...> class Target>
struct apply;

template<template<class...> class Target, template<class...> class TList, class... Ts>
struct apply<TList<Ts...>, Target>
{
    using type = Target<Ts...>;
};

template<class TList, template<class...> class Target>
using apply_t = typename apply<TList, Target>::type;

} //namespace

#endif
