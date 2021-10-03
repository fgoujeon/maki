//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_DETAIL_TLU_FOR_EACH_HPP
#define FGFSM_DETAIL_TLU_FOR_EACH_HPP

#include <utility>

namespace fgfsm::detail::tlu
{

/*
for_each is a fonction that takes a generic functor. It calls this functor for
each type in the given type list.

Since explicit-template-parameter-list notation doesn't exist yet for lambdas,
the type is given as a null type*.

For example, this call...:
    for_each<std::tuple<type0, type1, type2>>(f);

... is equivalent to these calls:
    f(static_cast<type0*>(nullptr));
    f(static_cast<type1*>(nullptr));
    f(static_cast<type2*>(nullptr));
*/

namespace for_each_detail
{
    template<class TypeList>
    struct helper;

    template<template<class...> class TypeList, class... Ts>
    struct helper<TypeList<Ts...>>
    {
        template<class F>
        static void call(F&& f)
        {
            (f(static_cast<Ts*>(nullptr)), ...);
        }
    };
}

template<class TypeList, class F>
void for_each(F&& f)
{
    for_each_detail::helper<TypeList>::call(std::forward<F>(f));
}

} //namespace

#endif
