//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_DETAIL_TLU_HEAD_HPP
#define FGFSM_DETAIL_TLU_HEAD_HPP

namespace fgfsm::detail::tlu
{

/*
head is the first type of the given type list.

In this example...:
    using type_list = std::tuple<char, short, int, long>;
    using first_t = head<type_list>;

... first_t is an alias of char.
*/

namespace head_detail
{
    template<class TList>
    struct head;

    template<template<class> class TList, class T, class... Ts>
    struct head<TList<T, Ts...>>
    {
        using type = T;
    };
}

template<class TList>
using head = typename head_detail::head<TList>::type;

} //namespace

#endif
