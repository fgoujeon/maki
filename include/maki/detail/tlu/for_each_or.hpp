//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_FOR_EACH_OR_HPP
#define MAKI_DETAIL_TLU_FOR_EACH_OR_HPP

namespace maki::detail::tlu
{

template<class TList, class F>
struct for_each_or_helper;

template<template<class...> class TList, class... Ts, class F>
struct for_each_or_helper<TList<Ts...>, F>
{
    template<class... Args>
    static bool call([[maybe_unused]] Args&... args)
    {
        return (F::template call<Ts>(args...) || ...);
    }
};

/*
Calls:
    return
        F::call<T0>(args...) ||
        F::call<T1>(args...) ||
        ...
        F::call<TN>(args...)
    ;
*/
template<class TList, class F, class... Args>
bool for_each_or(Args&... args)
{
    return for_each_or_helper<TList, F>::call(args...);
}

} //namespace

#endif
