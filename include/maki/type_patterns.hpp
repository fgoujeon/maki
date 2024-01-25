//Copyright Florian Goujeon 2021 - 2024.
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

#include "detail/constant.hpp"
#include "type.hpp"
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
template<const auto& Predicate>
struct any_if_t{};

/**
@brief A type pattern that matches with any type that verifies `Predicate<T>::value == false`.
@tparam Predicate the predicate against which types are tested
*/
template<const auto& Predicate>
struct any_if_not_t{};

/**
@brief A type pattern that matches with the given types.
@tparam Ts the types the pattern matches with
*/
template<const auto&... Values>
struct any_of_t{};

/**
@brief A type pattern that matches with any type but the given ones.
@tparam Ts the types the pattern doesn't match with
*/
template<const auto&... Values>
struct any_but_t{};

/**
@brief A type pattern that doesn't match with any type.
*/
struct none_t{};

/**
@}
*/

inline constexpr auto any = any_t{};

template<const auto& Predicate>
constexpr auto any_if = any_if_t<Predicate>{};

template<const auto& Predicate>
constexpr auto any_if_not = any_if_not_t<Predicate>{};

template<const auto&... Values>
constexpr auto any_of = any_of_t<Values...>{};

template<const auto&... Values>
constexpr auto any_but = any_but_t<Values...>{};

inline constexpr auto none = none_t{};


template<class... Types>
constexpr auto any_type_of = any_of<type<Types>...>;

template<class... Types>
constexpr auto any_type_but = any_but<type<Types>...>;


//matches_pattern
namespace detail
{
    //Pattern is a regular type
    template<class Value, class Pattern>
    constexpr bool matches_pattern(const Value& value, const Pattern& pattern)
    {
        if constexpr(std::is_same_v<Value, Pattern>)
        {
            return value == pattern;
        }
        else
        {
            return false;
        }
    }

    template<class Value>
    constexpr bool matches_pattern(const Value& /*value*/, const any_t /*pattern*/)
    {
        return true;
    }

    template<class Value, const auto& Predicate>
    constexpr bool matches_pattern(const Value& value, const any_if_t<Predicate> /*pattern*/)
    {
        return Predicate(value);
    }

    template<class Value, const auto& Predicate>
    constexpr bool matches_pattern(const Value& value, const any_if_not_t<Predicate> /*pattern*/)
    {
        return !Predicate(value);
    }

    template<class Value, const auto&... Values>
    constexpr bool matches_pattern(const Value& value, const any_of_t<Values...> /*pattern*/)
    {
        return (matches_pattern(value, Values) || ...);
    }

    template<class Value, const auto&... Values>
    constexpr bool matches_pattern(const Value& value, const any_but_t<Values...> /*pattern*/)
    {
        return (!matches_pattern(value, Values) && ...);
    }

    template<class Value>
    constexpr bool matches_pattern(const Value& /*value*/, const none_t /*pattern*/)
    {
        return false;
    }
}

namespace detail
{
    template<class T>
    constexpr bool is_type_pattern(const T& /*pattern*/)
    {
        return false;
    }

    inline constexpr bool is_type_pattern(const any_t /*pattern*/)
    {
        return true;
    }

    template<const auto& Predicate>
    constexpr bool is_type_pattern(const any_if_t<Predicate> /*pattern*/)
    {
        return true;
    }

    template<const auto& Predicate>
    constexpr bool is_type_pattern(const any_if_not_t<Predicate> /*pattern*/)
    {
        return true;
    }

    template<const auto&... Values>
    constexpr bool is_type_pattern(const any_of_t<Values...> /*pattern*/)
    {
        return true;
    }

    template<const auto&... Values>
    constexpr bool is_type_pattern(const any_but_t<Values...> /*pattern*/)
    {
        return true;
    }

    inline constexpr bool is_type_pattern(const none_t /*pattern*/)
    {
        return true;
    }

    template<class Value, class... Patterns>
    constexpr bool matches_any_pattern(const Value& value, const Patterns&... patterns)
    {
        return (matches_pattern(value, patterns) || ...);
    }
}

} //namespace

#endif
