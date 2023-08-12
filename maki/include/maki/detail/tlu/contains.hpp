//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_CONTAINS_HPP
#define MAKI_DETAIL_TLU_CONTAINS_HPP

namespace maki::detail::tlu
{

/*
contains is a boolean indicating whether the given typelist contains the given
type.

In this example...:
    using type_list = std::tuple<char, short, int, long>;
    constexpr auto contains_int = contains<type_list, int>;

... contains_int == true.
*/

namespace contains_detail
{
    template<class U, class... Ts>
    struct contains_in_type_pack;

    //terminal case
    template<class U>
    struct contains_in_type_pack<U>
    {
        static constexpr bool value = false;
    };

    //U == T
    template<class U, class... Ts>
    struct contains_in_type_pack<U, U, Ts...>
    {
        static constexpr bool value = true;
    };

    //U != T
    template<class U, class T, class... Ts>
    struct contains_in_type_pack<U, T, Ts...>
    {
        static constexpr bool value = contains_in_type_pack<U, Ts...>::value;
    };
}

template<class TList, class U>
struct contains;

template<template<class...> class TList, class... Ts, class U>
struct contains<TList<Ts...>, U>
{
    static constexpr bool value = contains_detail::contains_in_type_pack<U, Ts...>::value;
};

template<class TList, class U>
constexpr bool contains_v = contains<TList, U>::value;

} //namespace

#endif
