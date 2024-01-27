//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TU_STATIC_TO_CONSTANT_LIST_HPP
#define MAKI_DETAIL_TU_STATIC_TO_CONSTANT_LIST_HPP

#include "static_get_copy.hpp"
#include "../type_list.hpp"
#include "../../cref_constant.hpp"
#include <utility>

namespace maki::detail::tu
{

template<const auto& Tuple, class IndexSequence>
struct static_to_constant_list_impl;

template<const auto& Tuple, int... Indexes>
struct static_to_constant_list_impl<Tuple, std::integer_sequence<int, Indexes...>>
{
    using type = type_list<cref_constant_t<static_get_copy_v<Tuple, Indexes>>...>;
};

template<const auto& Tuple>
struct static_to_constant_list
{
    using type = typename static_to_constant_list_impl<Tuple, std::make_integer_sequence<int, Tuple.size>>::type;
};

template<const auto& Tuple>
using static_to_constant_list_t = typename static_to_constant_list<Tuple>::type;

} //namespace

#endif
