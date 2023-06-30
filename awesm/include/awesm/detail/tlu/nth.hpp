//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_TLU_NTH_HPP
#define AWESM_DETAIL_TLU_NTH_HPP

namespace awesm::detail::tlu
{

namespace nth_detail
{
    template<int Index, class... Ts>
    struct nth;

    template<int Index, class T, class... Ts>
    struct nth<Index, T, Ts...>
    {
        using type = typename nth<Index - 1, Ts...>::type;
    };

    template<class T, class... Ts>
    struct nth<0, T, Ts...>
    {
        using type = T;
    };
}

//Gets the Nth type of the type list

template<class TList, int Index>
struct nth;

template<template<class...> class TList, class... Ts, int Index>
struct nth<TList<Ts...>, Index>
{
    using type = typename nth_detail::nth<Index, Ts...>::type;
};

template<class TList, int Index>
using nth_t = typename nth<TList, Index>::type;

} //namespace

#endif
