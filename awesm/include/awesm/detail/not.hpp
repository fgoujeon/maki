//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_without_event.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_NOT_HPP
#define AWESM_DETAIL_NOT_HPP

namespace awesm::detail
{

//Higher order metafunction that applies 'not' to the value returned by the
//given metafunction
template<template<class> class F>
struct not_
{
    template<class T>
    struct type
    {
        static constexpr auto value = !F<T>::value;
    };
};

} //namespace

#endif
