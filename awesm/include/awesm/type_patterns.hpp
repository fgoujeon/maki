//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_TYPE_PATTERN_HPP
#define AWESM_TYPE_PATTERN_HPP

#include <type_traits>

namespace awesm
{

namespace detail
{
    struct type_pattern{};

    template<class T>
    constexpr auto is_type_pattern_v = std::is_base_of_v<type_pattern, T>;

    template<class TypePattern, class T>
    constexpr bool type_pattern_matches()
    {
        if constexpr(is_type_pattern_v<TypePattern>)
        {
            return TypePattern::template matches<T>;
        }
        else
        {
            return std::is_same_v<TypePattern, T>;
        }
    }
}

struct any: detail::type_pattern
{
    template<class T>
    static constexpr bool matches = true;
};

template<template<class> class Predicate>
struct any_if: detail::type_pattern
{
    template<class T>
    static constexpr bool matches = Predicate<T>::value;
};

template<template<class> class Predicate>
struct any_if_not: detail::type_pattern
{
    template<class T>
    static constexpr bool matches = !Predicate<T>::value;
};

template<class... Ts>
struct any_of: detail::type_pattern
{
    template<class T>
    static constexpr bool matches = (std::is_same_v<T, Ts> || ...);
};

template<class... Ts>
struct any_but: detail::type_pattern
{
    template<class T>
    static constexpr bool matches = !(std::is_same_v<T, Ts> || ...);
};

} //namespace

#endif
