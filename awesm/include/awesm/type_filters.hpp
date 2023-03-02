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

namespace detail
{
    struct type_filter{};

    template<class T, class Enable = void>
    struct is_type_filter
    {
        static constexpr auto value = false;
    };

    template<class T>
    struct is_type_filter<T, std::enable_if_t<std::is_void_v<typename T::type_filter_tag>>>
    {
        static constexpr auto value = true;
    };

    template<class T>
    constexpr auto is_type_filter_v = is_type_filter<T>::value;

    template<class T, class Filter>
    constexpr bool matches_filter_impl()
    {
        if constexpr(is_type_filter_v<Filter>)
        {
            return Filter::template matches<T>;
        }
        else
        {
            return std::is_same_v<T, Filter>;
        }
    }

    template<class T, class Filter>
    constexpr auto matches_filter_v = matches_filter_impl<T, Filter>();

    template<class T, class FilterList>
    struct matches_any_filter;

    template<class T, template<class...> class FilterList, class... Filters>
    struct matches_any_filter<T, FilterList<Filters...>>
    {
        static constexpr auto value = (matches_filter_v<T, Filters> || ...);
    };

    template<class T, class FilterList>
    constexpr auto matches_any_filter_v = matches_any_filter<T, FilterList>::value;
}

struct any
{
    using type_filter_tag = void;

    template<class T>
    static constexpr bool matches = true;
};

template<template<class> class Predicate>
struct any_if
{
    using type_filter_tag = void;

    template<class T>
    static constexpr bool matches = Predicate<T>::value;
};

template<template<class> class Predicate>
struct any_if_not
{
    using type_filter_tag = void;

    template<class T>
    static constexpr bool matches = !Predicate<T>::value;
};

template<class... Ts>
struct any_of
{
    using type_filter_tag = void;

    template<class T>
    static constexpr bool matches = (std::is_same_v<T, Ts> || ...);
};

template<class... Ts>
struct any_but
{
    using type_filter_tag = void;

    template<class T>
    static constexpr bool matches = !(std::is_same_v<T, Ts> || ...);
};

struct none
{
    using type_filter_tag = void;

    template<class T>
    static constexpr bool matches = false;
};

} //namespace

#endif
