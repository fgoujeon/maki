//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_CO_UTIL_HPP
#define MAKI_DETAIL_CO_UTIL_HPP

#ifdef __cpp_impl_coroutine
#include <coroutine>
#endif

namespace maki::detail::co_util
{

template<template<class> class Tpl>
struct awaitable_template_t
{
};

template<template<class> class Tpl>
constexpr auto awaitable_template = awaitable_template_t<Tpl>{};

template<class AwaitableTemplate, class T>
struct awaitable
{
    using type = T;
};

template<template<class> class Tpl, class T>
struct awaitable<awaitable_template_t<Tpl>, T>
{
    using type = Tpl<T>;
};

template<class AwaitableTemplate, class T>
using awaitable_t = typename awaitable<AwaitableTemplate, T>::type;

} //namespace

#endif
