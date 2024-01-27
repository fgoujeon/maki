//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TU_APPEND_HPP
#define MAKI_DETAIL_TU_APPEND_HPP

#include "get.hpp"
#include "../tuple.hpp"
#include <utility>

namespace maki::detail::tu
{

template<class IndexSequence>
struct append_impl;

template<int... Indexes>
struct append_impl<std::integer_sequence<int, Indexes...>>
{
    template<template<class...> class Tuple, class... Ts, class U>
    static constexpr auto call(const Tuple<Ts...>& tpl, const U& elem)
    {
        return Tuple<Ts..., U>{distributed_construct, get<Indexes>(tpl)..., elem};
    }
};

template<class Tuple, class U>
constexpr auto append(const Tuple& tpl, const U& elem)
{
    using impl_t = append_impl<std::make_integer_sequence<int, Tuple::size>>;
    return impl_t::call(tpl, elem);
}

} //namespace

#endif
