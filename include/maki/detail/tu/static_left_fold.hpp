//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TU_STATIC_LEFT_FOLD_HPP
#define MAKI_DETAIL_TU_STATIC_LEFT_FOLD_HPP

#include "static_get_copy.hpp"
#include "../../cref_constant.hpp"
#include <utility>

namespace maki::detail::tu
{

/*
static_left_fold

Applies a left fold on the given tuple.

Operation must be a generic lambda of this form:
    constexpr auto op = [](const auto& result_constant, const auto& element_constant)
    {
        //...
    };
*/

template
<
    const auto& Tuple,
    const auto& Operation,
    const auto& Initial,
    class IndexSequence
>
struct static_left_fold_impl;

template
<
    const auto& Tuple,
    const auto& Operation,
    const auto& Initial,
    int Index,
    int... Indexes
>
struct static_left_fold_impl<Tuple, Operation, Initial, std::integer_sequence<int, Index, Indexes...>>
{
    static constexpr auto next = Operation(cref_constant<Initial>, cref_constant<static_get_copy_v<Tuple, Index>>);

    static constexpr auto value = static_left_fold_impl
    <
        Tuple,
        Operation,
        next,
        std::integer_sequence<int, Indexes...>
    >::value;
};

template
<
    const auto& Tuple,
    const auto& Operation,
    const auto& Initial
>
struct static_left_fold_impl<Tuple, Operation, Initial, std::integer_sequence<int>>
{
    static constexpr auto value = Initial;
};

template
<
    const auto& Tuple,
    const auto& Operation,
    const auto& Initial
>
constexpr auto static_left_fold_v = static_left_fold_impl
<
    Tuple,
    Operation,
    Initial,
    std::make_integer_sequence<int, Tuple.size>
>::value;

} //namespace

#endif
