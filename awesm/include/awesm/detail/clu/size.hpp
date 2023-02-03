//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_CLU_SIZE_HPP
#define AWESM_DETAIL_CLU_SIZE_HPP

namespace awesm::detail::clu
{

template<class CList>
struct size;

template<template<auto...> class CList, auto... Cs>
struct size<CList<Cs...>>
{
    static constexpr auto value = static_cast<int>(sizeof...(Cs));
};

template<class CList>
constexpr int size_v = size<CList>::value;

} //namespace

#endif
