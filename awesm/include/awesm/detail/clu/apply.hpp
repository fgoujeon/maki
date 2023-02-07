//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_CLU_APPLY_HPP
#define AWESM_DETAIL_CLU_APPLY_HPP

namespace awesm::detail::clu
{

/*
apply_t<CList, Target> is an alias of Target<Cs...>, where Cs are the constants
of CList.
*/

template<class CList, template<auto...> class Target>
struct apply;

template<template<auto...> class Target, template<auto...> class CList, auto... Cs>
struct apply<CList<Cs...>, Target>
{
    using type = Target<Cs...>;
};

template<class CList, template<auto...> class Target>
using apply_t = typename apply<CList, Target>::type;

} //namespace

#endif
