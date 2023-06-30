//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_TLU_MAP_HPP
#define AWESM_DETAIL_TLU_MAP_HPP

namespace awesm::detail::tlu
{

template<class TList, template<class...> class Operation>
struct map;

template<template<class...> class TList, class... Ts, template<class> class Operation>
struct map<TList<Ts...>, Operation>
{
    using type = TList<Operation<Ts>...>;
};

template<class TList, template<class> class Operation>
using map_t = typename map<TList, Operation>::type;

} //namespace

#endif
