//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_TLU_SIZE_HPP
#define AWESM_DETAIL_TLU_SIZE_HPP

namespace awesm::detail::tlu
{

template<class TList>
struct size;

template<template<class...> class List, class... Ts>
struct size<List<Ts...>>
{
    static constexpr auto value = sizeof...(Ts);
};

template<class TList>
constexpr auto size_v = size<TList>::value;

} //namespace

#endif
