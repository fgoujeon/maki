//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_REMOVE_HPP
#define MAKI_DETAIL_TLU_REMOVE_HPP

#include "left_fold.hpp"
#include "push_back_if.hpp"
#include <type_traits>

namespace maki::detail::tlu
{

namespace remove_detail
{
    template<class Rhs>
    struct fold_operation_holder
    {
        template<class OutList, class Lhs>
        using operation = push_back_if_t
        <
            OutList,
            Lhs,
            !std::is_same_v<Lhs, Rhs>
        >;
    };
}

template<class LhsList, class Rhs>
struct remove;

template<template<class...> class LhsList, class... Lhss, class Rhs>
struct remove<LhsList<Lhss...>, Rhs>
{
    using type = left_fold_t
    <
        LhsList<Lhss...>,
        remove_detail::fold_operation_holder<Rhs>::template operation,
        LhsList<>
    >;
};

template<class LhsList, class Rhs>
using remove_t = typename remove<LhsList, Rhs>::type;

} //namespace

#endif
