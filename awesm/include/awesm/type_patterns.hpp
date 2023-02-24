//Copyright Florian Goujeon 2021 - 2023.
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

    template<class T, class Enable = void>
    struct is_type_pattern
    {
        static constexpr auto value = false;
    };

    template<class T>
    struct is_type_pattern<T, std::enable_if_t<std::is_void_v<typename T::type_pattern_tag>>>
    {
        static constexpr auto value = true;
    };

    template<class T>
    constexpr auto is_type_pattern_v = is_type_pattern<T>::value;

    template<class T, class Pattern>
    constexpr bool matches_pattern_impl()
    {
        if constexpr(is_type_pattern_v<Pattern>)
        {
            return Pattern::template matches<T>;
        }
        else
        {
            return std::is_same_v<T, Pattern>;
        }
    }

    template<class T, class Pattern>
    constexpr auto matches_pattern_v = matches_pattern_impl<T, Pattern>();
}

struct any
{
    using type_pattern_tag = void;

    template<class T>
    static constexpr bool matches = true;
};

template<template<class> class Predicate>
struct any_if
{
    using type_pattern_tag = void;

    template<class T>
    static constexpr bool matches = Predicate<T>::value;
};

template<template<class> class Predicate>
struct any_if_not
{
    using type_pattern_tag = void;

    template<class T>
    static constexpr bool matches = !Predicate<T>::value;
};

template<class... Ts>
struct any_of
{
    using type_pattern_tag = void;

    template<class T>
    static constexpr bool matches = (std::is_same_v<T, Ts> || ...);
};

template<class... Ts>
struct any_but
{
    using type_pattern_tag = void;

    template<class T>
    static constexpr bool matches = !(std::is_same_v<T, Ts> || ...);
};

} //namespace

#endif
