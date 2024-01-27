//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TU_GET_HPP
#define MAKI_DETAIL_TU_GET_HPP

#include "../constant.hpp"
#include "../../type.hpp"

namespace maki::detail::tu
{

template<int Index, class Tuple>
constexpr auto& get(Tuple& tpl)
{
    return tpl.get(constant<Index>);
}

template<class T, class Tuple>
constexpr auto& get(Tuple& tpl)
{
    return tpl.get(type<T>);
}

} //namespace

#endif
