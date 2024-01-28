//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TU_STATIC_FILTER_HPP
#define MAKI_DETAIL_TU_STATIC_FILTER_HPP

#include "append.hpp"
#include "static_left_fold.hpp"
#include "../tuple.hpp"

namespace maki::detail::tu
{

template<const auto& Predicate>
constexpr auto static_filter_op = [](const auto tuple_constant, const auto elem_constant)
{
    if constexpr(Predicate(elem_constant))
    {
        return append(tuple_constant.value, elem_constant.value);
    }
    else
    {
        return tuple_constant.value;
    }
};

template<const auto& Tuple, const auto& Predicate>
constexpr auto static_filter_v = static_left_fold_v
<
    Tuple,
    static_filter_op<Predicate>,
    empty_tuple
>;

} //namespace

#endif
