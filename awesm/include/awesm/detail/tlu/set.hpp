//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_TLU_SET_HPP
#define AWESM_DETAIL_TLU_SET_HPP

#include "push_front.hpp"
#include "apply.hpp"
#include "type_list.hpp"

namespace awesm::detail::tlu
{

namespace set_detail
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

    template<int ReverseIndex, class U, class... Ts>
    struct set_reverse;

    template<int ReverseIndex, class U, class T, class... Ts>
    struct set_reverse<ReverseIndex, U, T, Ts...>
    {
        using type = push_front_t
        <
            typename set_reverse<ReverseIndex, U, Ts...>::type,
            typename alternative<static_cast<int>(sizeof...(Ts)) == ReverseIndex, U, T>::type
        >;
    };

    template<int ReverseIndex, class U>
    struct set_reverse<ReverseIndex, U>
    {
        using type = type_list<>;
    };
}

template<class TList, int Index, class U>
class set;

template<template<class...> class TList, class... Ts, int Index, class U>
class set<TList<Ts...>, Index, U>
{
private:
    using temp_type = typename set_detail::set_reverse
    <
        static_cast<int>(sizeof...(Ts)) - Index - 1,
        U,
        Ts...
    >::type;

public:
    using type = apply_t<temp_type, TList>;
};

template<class TList, int Index, class U>
using set_t = typename set<TList, Index, U>::type;

} //namespace

#endif
