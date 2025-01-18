//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_INTEGER_CONSTANT_SEQUENCE_HPP
#define MAKI_DETAIL_INTEGER_CONSTANT_SEQUENCE_HPP

#include "constant.hpp"
#include "type_list.hpp"
#include <utility>
#include <cstdlib>

namespace maki::detail
{

template<class IntSequence>
struct make_integer_constant_sequence_impl;

template<class T, T... Is>
struct make_integer_constant_sequence_impl<std::integer_sequence<T, Is...>>
{
    using type = type_list_t<constant_t<Is>...>;
};

template<class T, std::size_t Size>
using make_integer_constant_sequence =
    typename make_integer_constant_sequence_impl
    <
        std::make_integer_sequence<T, Size>
    >::type
;

} //namespace

#endif
