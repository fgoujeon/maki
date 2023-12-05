//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the type pattern struct templates
*/

#ifndef MAKI_TYPE_PATTERNS_HPP
#define MAKI_TYPE_PATTERNS_HPP

#include "detail/state_conf_wrapper.hpp"
#include "detail/constant.hpp"
#include <type_traits>

namespace maki
{

/**
@defgroup TypePatterns Type Patterns
@brief Type patterns can be used in some places of the API (such as in transition
tables) in lieu of single types to concisely express a set of types.

@{
*/

/**
@brief A type pattern that matches with any type.
*/
struct any_t{};

/**
@brief A type pattern that matches with any type that verifies `Predicate<T>::value == true`.
@tparam Predicate the predicate against which types are tested
*/
template<template<class> class Predicate>
struct any_if_t{};

/**
@brief A type pattern that matches with any type that verifies `Predicate<T>::value == false`.
@tparam Predicate the predicate against which types are tested
*/
template<template<class> class Predicate>
struct any_if_not_t{};

/**
@brief A type pattern that matches with the given types.
@tparam Ts the types the pattern matches with
*/
template<class... Ts>
struct any_of_t{};

/**
@brief A type pattern that matches with any type but the given ones.
@tparam Ts the types the pattern doesn't match with
*/
template<class... Ts>
struct any_but_t{};

/**
@brief A type pattern that doesn't match with any type.
*/
struct none_t{};

/**
@}
*/

//TEMP
//variants for states

inline constexpr auto any = any_t{};

template<template<class> class Predicate>
constexpr auto any_if = any_if_t<Predicate>{};

template<template<class> class Predicate>
constexpr auto any_if_not = any_if_not_t<Predicate>{};

template<const auto&... StateConfs>
constexpr auto any_of = any_of_t<detail::state_conf_wrapper<StateConfs>...>{};

template<const auto&... StateConfs>
constexpr auto any_but = any_but_t<detail::state_conf_wrapper<StateConfs>...>{};

inline constexpr auto none = none_t{};

//matches_pattern
namespace detail
{
    //Pattern is a regular type
    template<class T, class Pattern>
    struct matches_pattern
    {
        static constexpr bool regular = true;
        static constexpr bool value = std::is_same_v<T, Pattern>;
    };

    template<class T, class Pattern>
    constexpr auto matches_pattern_v = matches_pattern<T, Pattern>::value;

    template<class T>
    struct matches_pattern<T, any_t>
    {
        static constexpr bool value = true;
    };

    template<class T, template<class> class Predicate>
    struct matches_pattern<T, any_if_t<Predicate>>
    {
        static constexpr bool value = Predicate<T>::value;
    };

    template<class T, template<class> class Predicate>
    struct matches_pattern<T, any_if_not_t<Predicate>>
    {
        static constexpr bool value = !Predicate<T>::value;
    };

    template<class T, class... Ts>
    struct matches_pattern<T, any_of_t<Ts...>>
    {
        //MSVC wants a function for the fold expression
        static constexpr bool make_value()
        {
            return (matches_pattern_v<T, Ts> || ...);
        }

        static constexpr bool value = make_value();
    };

    template<class T, class... Ts>
    struct matches_pattern<T, any_but_t<Ts...>>
    {
        //MSVC wants a function for the fold expression
        static constexpr bool make_value()
        {
            return (!matches_pattern_v<T, Ts> && ...);
        }

        static constexpr bool value = make_value();
    };

    template<class T>
    struct matches_pattern<T, none_t>
    {
        static constexpr bool value = false;
    };
}

namespace detail
{
    template<class T, class Enable = void>
    struct is_type_pattern
    {
        static constexpr auto value = true;
    };

    template<class T>
    struct is_type_pattern
    <
        T,
        std::enable_if_t<matches_pattern<void, T>::regular>
    >
    {
        static constexpr auto value = false;
    };

    //TEMP
    template<class T>
    struct is_type_pattern_for_state
    {
        static constexpr auto value = false;
    };

    //TEMP
    template<const auto& Value>
    struct is_type_pattern_for_state<detail::constant<Value>>
    {
        using value_type = std::decay_t<decltype(Value)>;
        static constexpr auto value = is_type_pattern<value_type>::value;
    };

    template<class T>
    constexpr auto is_type_pattern_v = is_type_pattern<T>::value || is_type_pattern_for_state<T>::value;

    template<class T, class PatternList>
    class matches_any_pattern;

    template<class T, template<class...> class PatternList, class... Patterns>
    class matches_any_pattern<T, PatternList<Patterns...>>
    {
    private:
        //MSVC wants a function for the fold expression
        static constexpr bool make_value()
        {
            return (matches_pattern<T, Patterns>::value || ...);
        }

    public:
        static constexpr auto value = make_value();
    };

    template<class T, class PatternList>
    constexpr auto matches_any_pattern_v = matches_any_pattern<T, PatternList>::value;
}

} //namespace

#endif
