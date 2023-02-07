//Copyright Florian Goujeon 2021 - 2023.
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

template<template<class...> class TList, class... Ts>
struct size<TList<Ts...>>
{
    static constexpr auto value = static_cast<int>(sizeof...(Ts));
};

template<class TList>
constexpr int size_v = size<TList>::value;

} //namespace

#endif
