//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_FOR_EACH_HPP
#define MAKI_DETAIL_TLU_FOR_EACH_HPP

namespace maki::detail::tlu
{

template<class TList, class F>
struct for_each_helper;

template<template<class...> class TList, class... Ts, class F>
struct for_each_helper<TList<Ts...>, F>
{
    template<class... Args>
    static void call([[maybe_unused]] Args&... args)
    {
        (F::template call<Ts>(args...), ...);
    }
};

/*
Calls:
    F::call<T0>(args...);
    F::call<T1>(args...);
    ...
    F::call<TN>(args...);
*/
template<class TList, class F, class... Args>
void for_each(Args&... args)
{
    for_each_helper<TList, F>::call(args...);
}

} //namespace

#endif
