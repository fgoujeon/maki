// Copyright Florian Goujeon 2021 - 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
// Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_CONTAINS_HPP
#define MAKI_DETAIL_TLU_CONTAINS_HPP

#include <type_traits>

namespace maki::detail::tlu
{

/*
contains is a boolean indicating whether the given typelist contains the given
type.

In this example...:
    using type_list_t = std::tuple<char, short, int, long>;
    constexpr auto contains_int = contains<type_list_t, int>;

... contains_int == true.
*/

template<class TList, class U>
struct contains;

template<template<class...> class TList, class... Ts, class U>
struct contains<TList<Ts...>, U>
{
    static constexpr bool value = (std::is_same_v<Ts, U> || ...);
};

template<class TList, class U>
constexpr bool contains_v = contains<TList, U>::value;

} // namespace maki::detail::tlu

#endif
