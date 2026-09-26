// Copyright Florian Goujeon 2021 - 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
// Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_MAP_HPP
#define MAKI_DETAIL_TLU_MAP_HPP

namespace maki::detail::tlu
{

/*
`map_t<F, some_template<Ts...>>` is an alias of `some_template<F<Ts>...>`.
*/

template<template<class> class F, class TList>
struct map;

template<template<class> class F, template<class...> class TList, class... Ts>
struct map<F, TList<Ts...>>
{
    using type = TList<F<Ts>...>;
};

template<template<class> class F, class TList>
using map_t = typename map<F, TList>::type;

} // namespace maki::detail::tlu

#endif
