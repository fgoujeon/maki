//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_TLU_SET_HPP
#define AWESM_DETAIL_TLU_SET_HPP

#include "push_front.hpp"
#include "size.hpp"

namespace awesm::detail::tlu
{

namespace set_at_detail
{
    template<bool B, class T, class F>
    struct alternative
    {
        using type = T;
    };

    template<class T, class F>
    struct alternative<false, T, F>
    {
        using type = F;
    };

    template<template<class...> class TList, int ReverseIndex, class U, class... Ts>
    struct set_at_reverse;

    template<template<class...> class TList, int ReverseIndex, class U, class T, class... Ts>
    struct set_at_reverse<TList, ReverseIndex, U, T, Ts...>
    {
        using type = push_front_t
        <
            typename set_at_reverse<TList, ReverseIndex, U, Ts...>::type,
            typename alternative<sizeof...(Ts) == ReverseIndex, U, T>::type
        >;
    };

    template<template<class...> class TList, int ReverseIndex, class U>
    struct set_at_reverse<TList, ReverseIndex, U>
    {
        using type = TList<>;
    };
}

template<class TList, int Index, class U>
struct set_at;

template<template<class...> class TList, class... Ts, int Index, class U>
struct set_at<TList<Ts...>, Index, U>
{
    using type = typename set_at_detail::set_at_reverse
    <
        TList,
        sizeof...(Ts) - Index - 1,
        U,
        Ts...
    >::type;
};

template<class TList, int Index, class U>
using set_at_t = typename set_at<TList, Index, U>::type;

template<class TList, auto Index, class U>
using set_at_f_t = set_at_t<TList, static_cast<int>(Index), U>;

} //namespace

#endif
