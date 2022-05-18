//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_TYPE_PATTERN_HPP
#define FGFSM_TYPE_PATTERN_HPP

namespace fgfsm
{

namespace detail
{
    struct type_pattern{};
}

struct any: detail::type_pattern
{
    template<class T>
    static constexpr auto matches = true;
};

template<template<class> class Predicate>
struct any_if: detail::type_pattern
{
    template<class T>
    static constexpr auto matches = Predicate<T>::value;
};

template<template<class> class Predicate>
struct any_if_not: detail::type_pattern
{
    template<class T>
    static constexpr auto matches = !Predicate<T>::value;
};

template<class... Ts>
struct any_of: detail::type_pattern
{
    template<class T>
    static constexpr auto matches = (std::is_same_v<T, Ts> || ...);
};

template<class... Ts>
struct any_but: detail::type_pattern
{
    template<class T>
    static constexpr auto matches = !(std::is_same_v<T, Ts> || ...);
};

} //namespace

#endif
