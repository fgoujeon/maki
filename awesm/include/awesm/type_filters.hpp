//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_TYPE_FILTERS_HPP
#define AWESM_TYPE_FILTERS_HPP

#include <type_traits>

namespace awesm
{

/**
@defgroup TypeFilters Type Filters
Type filters can be used in some places of the API (such as in transition
tables) in lieu of single types to concisely express a set of types.
@{
*/

/**
A type filter that matches with any type.
*/
struct any{};

/**
A type filter that matches with any type that verifies `Predicate<T>::value == true`.
@tparam Predicate the predicate against which types are tested
*/
template<template<class> class Predicate>
struct any_if{};

/**
A type filter that matches with any type that verifies `Predicate<T>::value == false`.
@tparam Predicate the predicate against which types are tested
*/
template<template<class> class Predicate>
struct any_if_not{};

/**
A type filter that matches with the given types.
@tparam Ts the types the filter matches with
*/
template<class... Ts>
struct any_of{};

/**
A type filter that matches with any type but the given ones.
@tparam Ts the types the filter doesn't match with
*/
template<class... Ts>
struct any_but{};

/**
A type filter that doesn't match with any type.
*/
struct none{};

/**
@}
*/

//matches_filter
namespace detail
{
    //Filter is a regular type
    template<class T, class Filter>
    struct matches_filter
    {
        static constexpr bool regular = true;
        static constexpr bool value = std::is_same_v<T, Filter>;
    };

    template<class T>
    struct matches_filter<T, any>
    {
        static constexpr bool value = true;
    };

    template<class T, template<class> class Predicate>
    struct matches_filter<T, any_if<Predicate>>
    {
        static constexpr bool value = Predicate<T>::value;
    };

    template<class T, template<class> class Predicate>
    struct matches_filter<T, any_if_not<Predicate>>
    {
        static constexpr bool value = !Predicate<T>::value;
    };

    template<class T, class... Ts>
    struct matches_filter<T, any_of<Ts...>>
    {
        //MSVC wants a function for the fold expression
        static constexpr bool make_value()
        {
            return (std::is_same_v<T, Ts> || ...);
        }

        static constexpr bool value = make_value();
    };

    template<class T, class... Ts>
    struct matches_filter<T, any_but<Ts...>>
    {
        //MSVC wants a function for the fold expression
        static constexpr bool make_value()
        {
            return !(std::is_same_v<T, Ts> || ...);
        }

        static constexpr bool value = make_value();
    };

    template<class T>
    struct matches_filter<T, none>
    {
        static constexpr bool value = false;
    };

    template<class T, class Filter>
    constexpr auto matches_filter_v = matches_filter<T, Filter>::value;
}

namespace detail
{
    template<class T, class Enable = void>
    struct is_type_filter
    {
        static constexpr auto value = true;
    };

    template<class T>
    struct is_type_filter
    <
        T,
        std::enable_if_t<matches_filter<void, T>::regular>
    >
    {
        static constexpr auto value = false;
    };

    template<class T>
    constexpr auto is_type_filter_v = is_type_filter<T>::value;

    template<class T, class FilterList>
    class matches_any_filter;

    template<class T, template<class...> class FilterList, class... Filters>
    class matches_any_filter<T, FilterList<Filters...>>
    {
    private:
        //MSVC wants a function for the fold expression
        static constexpr bool make_value()
        {
            return (matches_filter<T, Filters>::value || ...);
        }

    public:
        static constexpr auto value = make_value();
    };

    template<class T, class FilterList>
    constexpr auto matches_any_filter_v = matches_any_filter<T, FilterList>::value;
}

} //namespace

#endif
