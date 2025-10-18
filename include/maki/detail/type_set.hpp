//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TYPE_SET_HPP
#define MAKI_DETAIL_TYPE_SET_HPP

#include "tlu/remove_all.hpp"
#include "tlu/remove.hpp"
#include "tlu/intersection.hpp"
#include "tlu/push_back_all_unique.hpp"
#include "tlu/push_back_unique.hpp"
#include "tlu/contains.hpp"

namespace maki::detail
{

/*
Type set implementations
*/

/*
A set containing a single type `T`.
This could be an alias of `type_set_inclusion_list<T>`, but this simple
implementation allows for faster builds.
*/
template<class T>
struct type_set_item{};

// A set containing types in `Ts`.
template<class... Ts>
struct type_set_inclusion_list{};

// A set containing all types but the ones in `Ts...`
template<class... Ts>
struct type_set_exclusion_list{};


/*
Predefined type sets
*/

using empty_type_set_t = type_set_inclusion_list<>;

using universal_type_set_t = type_set_exclusion_list<>;


/*
`type_set_contains`
*/

template<class Impl, class T>
struct type_set_contains;

template<class T, class U>
struct type_set_contains<type_set_item<T>, U>
{
    static constexpr bool value = std::is_same_v<T, U>;
};

template<class... Ts, class T>
struct type_set_contains<type_set_inclusion_list<Ts...>, T>
{
    static constexpr bool value = tlu::contains_v<type_set_inclusion_list<Ts...>, T>;
};

template<class... Ts, class T>
struct type_set_contains<type_set_exclusion_list<Ts...>, T>
{
    static constexpr bool value = !tlu::contains_v<type_set_exclusion_list<Ts...>, T>;
};

template<class Impl, class T>
constexpr auto type_set_contains_v = type_set_contains<Impl, T>::value;


/*
`type_set_inverse`
*/

template<class Impl>
struct type_set_inverse;

template<class... Ts>
struct type_set_inverse<type_set_inclusion_list<Ts...>>
{
    using type = type_set_exclusion_list<Ts...>;
};

template<class... Ts>
struct type_set_inverse<type_set_exclusion_list<Ts...>>
{
    using type = type_set_inclusion_list<Ts...>;
};

template<class Impl>
using type_set_inverse_t = typename type_set_inverse<Impl>::type;


/*
`type_set_union`
*/

template<class Lhs, class Rhs>
struct type_set_union;

template<class Lhs, class Rhs>
struct type_set_union<type_set_item<Lhs>, type_set_item<Rhs>>
{
    using type = type_set_inclusion_list<Lhs, Rhs>;
};

template<class Lhs, class... Rhss>
struct type_set_union<type_set_item<Lhs>, type_set_inclusion_list<Rhss...>>
{
    using type = tlu::push_back_unique_t
    <
        type_set_inclusion_list<Rhss...>,
        Lhs
    >;
};

template<class Lhs, class... Rhss>
struct type_set_union<type_set_item<Lhs>, type_set_exclusion_list<Rhss...>>
{
    using type = tlu::remove_t
    <
        type_set_exclusion_list<Rhss...>,
        Lhs
    >;
};

template<class... Lhss, class Rhs>
struct type_set_union<type_set_inclusion_list<Lhss...>, type_set_item<Rhs>>
{
    using type = tlu::push_back_unique_t
    <
        type_set_inclusion_list<Lhss...>,
        Rhs
    >;
};

template<class... Lhss, class... Rhss>
struct type_set_union<type_set_inclusion_list<Lhss...>, type_set_inclusion_list<Rhss...>>
{
    using type = tlu::push_back_all_unique_t
    <
        type_set_inclusion_list<Lhss...>,
        type_set_inclusion_list<Rhss...>
    >;
};

template<class... Lhss, class... Rhss>
struct type_set_union<type_set_inclusion_list<Lhss...>, type_set_exclusion_list<Rhss...>>
{
    using type = tlu::remove_all_t
    <
        type_set_exclusion_list<Rhss...>,
        type_set_inclusion_list<Lhss...>
    >;
};

template<class... Lhss, class Rhs>
struct type_set_union<type_set_exclusion_list<Lhss...>, type_set_item<Rhs>>
{
    using type = tlu::remove_t
    <
        type_set_exclusion_list<Lhss...>,
        Rhs
    >;
};

template<class... Lhss, class... Rhss>
struct type_set_union<type_set_exclusion_list<Lhss...>, type_set_inclusion_list<Rhss...>>
{
    using type = tlu::remove_all_t
    <
        type_set_exclusion_list<Lhss...>,
        type_set_inclusion_list<Rhss...>
    >;
};

template<class... Lhss, class... Rhss>
struct type_set_union<type_set_exclusion_list<Lhss...>, type_set_exclusion_list<Rhss...>>
{
    using type = tlu::intersection_t
    <
        type_set_exclusion_list<Lhss...>,
        type_set_exclusion_list<Rhss...>
    >;
};

template<class Lhs, class Rhs>
using type_set_union_t = typename type_set_union<Lhs, Rhs>::type;


/*
`type_set_intersection`
*/

template<class Lhs, class Rhs>
struct type_set_intersection;

template<class... Lhss, class... Rhss>
struct type_set_intersection<type_set_inclusion_list<Lhss...>, type_set_inclusion_list<Rhss...>>
{
    using type = tlu::intersection_t
    <
        type_set_inclusion_list<Lhss...>,
        type_set_inclusion_list<Rhss...>
    >;
};

template<class... Lhss, class... Rhss>
struct type_set_intersection<type_set_inclusion_list<Lhss...>, type_set_exclusion_list<Rhss...>>
{
    using type = tlu::remove_all_t
    <
        type_set_inclusion_list<Lhss...>,
        type_set_exclusion_list<Rhss...>
    >;
};

template<class... Lhss, class... Rhss>
struct type_set_intersection<type_set_exclusion_list<Lhss...>, type_set_inclusion_list<Rhss...>>
{
    using type = tlu::remove_all_t
    <
        type_set_inclusion_list<Rhss...>,
        type_set_exclusion_list<Lhss...>
    >;
};

template<class... Lhss, class... Rhss>
struct type_set_intersection<type_set_exclusion_list<Lhss...>, type_set_exclusion_list<Rhss...>>
{
    using type = tlu::push_back_all_unique_t
    <
        type_set_exclusion_list<Lhss...>,
        type_set_exclusion_list<Rhss...>
    >;
};

template<class Lhs, class Rhs>
using type_set_intersection_t = typename type_set_intersection<Lhs, Rhs>::type;

} //namespace

#endif
