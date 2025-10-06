//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_INTERSECTION_HPP
#define MAKI_DETAIL_TLU_INTERSECTION_HPP

#include "left_fold.hpp"
#include "push_back_if.hpp"
#include "contains.hpp"

namespace maki::detail::tlu
{

namespace intersection_detail
{
    template<class LhsList>
    struct fold_operation_holder
    {
        template<class OutList, class Rhs>
        using operation = push_back_if_t
        <
            OutList,
            Rhs,
            contains_v<LhsList, Rhs>
        >;
    };
}

template<class LhsList, class RhsList>
struct intersection;

template<template<class...> class LhsList, class... Lhss, class RhsList>
struct intersection<LhsList<Lhss...>, RhsList>
{
    using type = left_fold_t
    <
        RhsList,
        intersection_detail::fold_operation_holder<LhsList<Lhss...>>::template operation,
        LhsList<>
    >;
};

template<class LhsList, class RhsList>
using intersection_t = typename intersection<LhsList, RhsList>::type;

} //namespace

#endif
