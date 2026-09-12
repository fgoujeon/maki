//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TYPE_TEMPLATE_HPP
#define MAKI_DETAIL_TYPE_TEMPLATE_HPP

namespace maki::detail
{

// A holder for a type template with one type as template argument.
template<template<class> class Tpl>
struct type_template
{
    template<class T>
    using type = Tpl<T>;
};

} //namespace

#endif
