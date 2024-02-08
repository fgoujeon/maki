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

#include "detail/storable_conf_pattern.hpp"
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
template<class Predicate>
struct any_if_t
{
    Predicate pred;
};

/**
@brief A type pattern that matches with any type that verifies `Predicate<T>::value == false`.
@tparam Predicate the predicate against which types are tested
*/
template<class Predicate>
struct any_if_not_t
{
    Predicate pred;
};

/**
@brief A type pattern that matches with the given types.
@tparam Ts the types the pattern matches with
*/
template<class... Ts>
struct any_of_t
{
    detail::tuple<Ts...> values;
};

/**
@brief A type pattern that matches with any type but the given ones.
@tparam Ts the types the pattern doesn't match with
*/
template<class... Ts>
struct any_but_t
{
    detail::tuple<Ts...> values;
};

/**
@brief A type pattern that doesn't match with any type.
*/
struct none_t{};

/**
@}
*/

inline constexpr auto any = any_t{};

template<class Predicate>
constexpr auto any_if(const Predicate& pred)
{
    return any_if_t<Predicate>{pred};
}

template<class Predicate>
constexpr auto any_if_not(const Predicate& pred)
{
    return any_if_not_t<Predicate>{pred};
}

template<class... Ts>
constexpr auto any_of(const Ts&... values)
{
    return any_of_t<detail::storable_conf_pattern_t<Ts>...>
    {
        detail::tuple<detail::storable_conf_pattern_t<Ts>...>
        {
            detail::distributed_construct,
            detail::to_storable_conf_pattern(values)...
        }
    };
}

template<class... Ts>
constexpr auto any_but(const Ts&... values)
{
    return any_but_t<detail::storable_conf_pattern_t<Ts>...>
    {
        detail::tuple<detail::storable_conf_pattern_t<Ts>...>
        {
            detail::distributed_construct,
            detail::to_storable_conf_pattern(values)...
        }
    };
}

inline constexpr auto none = none_t{};


template<class... Types>
constexpr auto any_type_of = any_of(type<Types>...);

template<class... Types>
constexpr auto any_type_but = any_but(type<Types>...);


//matches_pattern
namespace detail
{
    //Pattern is a regular type
    template<class Value, class Pattern>
    constexpr bool matches_pattern(const Value& value, const Pattern& pattern)
    {
        return value == pattern;
    }

    template<class Value>
    constexpr bool matches_pattern(const Value& /*value*/, const any_t /*pattern*/)
    {
        return true;
    }

    template<class Value, class Predicate>
    constexpr bool matches_pattern(const Value& value, const any_if_t<Predicate>& pattern)
    {
        return pattern.pred(value);
    }

    template<class Value, class Predicate>
    constexpr bool matches_pattern(const Value& value, const any_if_not_t<Predicate>& pattern)
    {
        return !pattern.pred(value);
    }

    template<class Value, class... Ts>
    constexpr bool matches_pattern(const Value& value, const any_of_t<Ts...>& pattern)
    {
        return tuple_apply
        (
            pattern.values,
            [](const Value& value, const Ts&... values)
            {
                return (matches_pattern(value, values) || ...);
            },
            value
        );
    }

    template<class Value, class... Ts>
    constexpr bool matches_pattern(const Value& value, const any_but_t<Ts...>& pattern)
    {
        return tuple_apply
        (
            pattern.values,
            [](const Value& value, const Ts&... values)
            {
                return (!matches_pattern(value, values) && ...);
            },
            value
        );
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
    struct is_type_pattern
    {
        static constexpr auto value = false;
    };

    template<class T>
    constexpr auto is_type_pattern_v = is_type_pattern<T>::value;

    template<>
    struct is_type_pattern<any_t>
    {
        static constexpr auto value = true;
    };

    template<class Predicate>
    struct is_type_pattern<any_if_t<Predicate>>
    {
        static constexpr auto value = true;
    };

    template<class Predicate>
    struct is_type_pattern<any_if_not_t<Predicate>>
    {
        static constexpr auto value = true;
    };

    template<class... Ts>
    struct is_type_pattern<any_of_t<Ts...>>
    {
        static constexpr auto value = true;
    };

    template<class... Ts>
    struct is_type_pattern<any_but_t<Ts...>>
    {
        static constexpr auto value = true;
    };

    template<>
    struct is_type_pattern<none_t>
    {
        static constexpr auto value = true;
    };

    template<class Value, class... Patterns>
    constexpr bool matches_any_pattern(const Value& value, const Patterns&... patterns)
    {
        return (matches_pattern(value, patterns) || ...);
    }
}

} //namespace

#endif
