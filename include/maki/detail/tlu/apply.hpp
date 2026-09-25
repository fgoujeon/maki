// Copyright Florian Goujeon 2021 - 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
// Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_APPLY_HPP
#define MAKI_DETAIL_TLU_APPLY_HPP

namespace maki::detail::tlu
{

/*
`apply_t<Target, some_template<Ts...>>` is an alias of `Target<Ts...>`.
*/

template<template<class...> class Target, class TList>
struct apply;

template<
    template<class...> class TList,
    class... Ts,
    template<class...> class Target>
struct apply<Target, TList<Ts...>>
{
    using type = Target<Ts...>;
};

template<template<class...> class Target, class TList>
using apply_t = typename apply<Target, TList>::type;

} // namespace maki::detail::tlu

#endif
