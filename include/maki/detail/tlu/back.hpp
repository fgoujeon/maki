//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_BACK_HPP
#define MAKI_DETAIL_TLU_BACK_HPP

namespace maki::detail::tlu
{

namespace back_detail
{
    template<class... Ts>
    struct type_pack_back;

    template<class T0, class T1, class... Ts>
    struct type_pack_back<T0, T1, Ts...>
    {
        using type = type_pack_back<T1, Ts...>;
    };

    template<class T>
    struct type_pack_back<T>
    {
        using type = T;
    };
}

template<class TList>
struct back;

template<template<class...> class TList, class... Ts>
struct back<TList<Ts...>>
{
    using type = back_detail::type_pack_back<Ts...>;
};

template<class TList>
using back_t = back<TList>;

} //namespace

#endif
