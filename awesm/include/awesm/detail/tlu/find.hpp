//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_TLU_FIND_HPP
#define AWESM_DETAIL_TLU_FIND_HPP

#include <type_traits>

namespace awesm::detail::tlu
{

template<class TList, template<class> class Predicate, class Default, class Enable = void>
struct find;

template<template<class...> class TList, class T, class... Ts, template<class> class Predicate, class Default>
struct find<TList<T, Ts...>, Predicate, Default, std::enable_if_t<Predicate<T>::value>>
{
    using type = T;
};

template<template<class...> class TList, class T, class... Ts, template<class> class Predicate, class Default>
struct find<TList<T, Ts...>, Predicate, Default, std::enable_if_t<!Predicate<T>::value>>
{
    using type = typename find<TList<Ts...>, Predicate, Default>::type;
};

template<template<class...> class TList, template<class> class Predicate, class Default>
struct find<TList<>, Predicate, Default>
{
    using type = Default;
};

template<class TList, template<class> class Predicate, class Default>
using find_t = typename find<TList, Predicate, Default>::type;

} //namespace

#endif
