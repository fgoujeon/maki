//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TU_CONTAINS_IF_HPP
#define MAKI_DETAIL_TU_CONTAINS_IF_HPP

#include "apply.hpp"

namespace maki::detail::tu
{

inline constexpr auto contains_if_impl = [](const auto& pred, const auto&... elems)
{
    return (pred(elems) || ...);
};

template<class Tuple, class Predicate>
constexpr bool contains_if(Tuple& tpl, const Predicate& pred)
{
    return apply(tpl, contains_if_impl, pred);
}

} //namespace

#endif
