//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TYPE_SET_IMPLS_HPP
#define MAKI_DETAIL_TYPE_SET_IMPLS_HPP

#include "tlu/remove_all.hpp"
#include "tlu/remove.hpp"
#include "tlu/intersection.hpp"
#include "tlu/push_back_all_unique.hpp"
#include "tlu/push_back_unique.hpp"
#include "tlu/contains.hpp"

/*
Implementation types for `maki::event_set`.
*/

namespace maki::detail::type_set_impls
{

// A set containing a single type `T`.
template<class T>
struct item{};

// A set containing types in `Ts`.
template<class... Ts>
struct inclusion_list{};

// A set containing all types but the ones in `Ts...`
template<class... Ts>
struct exclusion_list{};


/*
`contains`
*/

template<class Impl, class T>
struct contains;

template<class T, class U>
struct contains<item<T>, U>
{
    static constexpr bool value = std::is_same_v<T, U>;
};

template<class... Ts, class T>
struct contains<inclusion_list<Ts...>, T>
{
    static constexpr bool value = tlu::contains_v<inclusion_list<Ts...>, T>;
};

template<class... Ts, class T>
struct contains<exclusion_list<Ts...>, T>
{
    static constexpr bool value = !tlu::contains_v<exclusion_list<Ts...>, T>;
};

template<class Impl, class T>
constexpr auto contains_v = contains<Impl, T>::value;


/*
`inverse_of`
*/

template<class Impl>
struct inverse_of;

template<class... Ts>
struct inverse_of<inclusion_list<Ts...>>
{
    using type = exclusion_list<Ts...>;
};

template<class... Ts>
struct inverse_of<exclusion_list<Ts...>>
{
    using type = inclusion_list<Ts...>;
};

template<class Impl>
using inverse_of_t = typename inverse_of<Impl>::type;


/*
`union_of`
*/

template<class Lhs, class Rhs>
struct union_of;

template<class Lhs, class Rhs>
struct union_of<item<Lhs>, item<Rhs>>
{
    using type = inclusion_list<Lhs, Rhs>;
};

template<class Lhs, class... Rhss>
struct union_of<item<Lhs>, inclusion_list<Rhss...>>
{
    using type = tlu::push_back_unique_t
    <
        inclusion_list<Rhss...>,
        Lhs
    >;
};

template<class Lhs, class... Rhss>
struct union_of<item<Lhs>, exclusion_list<Rhss...>>
{
    using type = tlu::remove_t
    <
        exclusion_list<Rhss...>,
        Lhs
    >;
};

template<class... Lhss, class Rhs>
struct union_of<inclusion_list<Lhss...>, item<Rhs>>
{
    using type = tlu::push_back_unique_t
    <
        inclusion_list<Lhss...>,
        Rhs
    >;
};

template<class... Lhss, class... Rhss>
struct union_of<inclusion_list<Lhss...>, inclusion_list<Rhss...>>
{
    using type = tlu::push_back_all_unique_t
    <
        inclusion_list<Lhss...>,
        inclusion_list<Rhss...>
    >;
};

template<class... Lhss, class... Rhss>
struct union_of<inclusion_list<Lhss...>, exclusion_list<Rhss...>>
{
    using type = tlu::remove_all_t
    <
        exclusion_list<Rhss...>,
        inclusion_list<Lhss...>
    >;
};

template<class... Lhss, class Rhs>
struct union_of<exclusion_list<Lhss...>, item<Rhs>>
{
    using type = tlu::remove_t
    <
        exclusion_list<Lhss...>,
        Rhs
    >;
};

template<class... Lhss, class... Rhss>
struct union_of<exclusion_list<Lhss...>, inclusion_list<Rhss...>>
{
    using type = tlu::remove_all_t
    <
        exclusion_list<Lhss...>,
        inclusion_list<Rhss...>
    >;
};

template<class... Lhss, class... Rhss>
struct union_of<exclusion_list<Lhss...>, exclusion_list<Rhss...>>
{
    using type = tlu::intersection_t
    <
        exclusion_list<Lhss...>,
        exclusion_list<Rhss...>
    >;
};

template<class Lhs, class Rhs>
using union_of_t = typename union_of<Lhs, Rhs>::type;


/*
`intersection_of`
*/

template<class Lhs, class Rhs>
struct intersection_of;

template<class... Lhss, class... Rhss>
struct intersection_of<inclusion_list<Lhss...>, inclusion_list<Rhss...>>
{
    using type = tlu::intersection_t
    <
        inclusion_list<Lhss...>,
        inclusion_list<Rhss...>
    >;
};

template<class... Lhss, class... Rhss>
struct intersection_of<inclusion_list<Lhss...>, exclusion_list<Rhss...>>
{
    using type = tlu::remove_all_t
    <
        inclusion_list<Lhss...>,
        exclusion_list<Rhss...>
    >;
};

template<class... Lhss, class... Rhss>
struct intersection_of<exclusion_list<Lhss...>, inclusion_list<Rhss...>>
{
    using type = tlu::remove_all_t
    <
        inclusion_list<Rhss...>,
        exclusion_list<Lhss...>
    >;
};

template<class... Lhss, class... Rhss>
struct intersection_of<exclusion_list<Lhss...>, exclusion_list<Rhss...>>
{
    using type = tlu::push_back_all_unique_t
    <
        exclusion_list<Lhss...>,
        exclusion_list<Rhss...>
    >;
};

template<class Lhs, class Rhs>
using intersection_of_t = typename intersection_of<Lhs, Rhs>::type;

} //namespace

#endif
