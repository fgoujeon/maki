//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_SET_HPP
#define MAKI_SET_HPP

#include "tuple.hpp"
#include "equals.hpp"
#include "type.hpp"
#include "tlu/push_back_all_unique.hpp"
#include "tlu/push_back_unique.hpp"
#include "tlu/contains.hpp"

namespace maki::detail
{

/*
Implementation types for `maki::event_set` and `maki::state_set`.
*/

/*
predicate_based_set

Most generic way to express a set.
*/

template<class Predicate>
struct predicate_based_set
{
    Predicate predicate;
};

template<class Predicate>
predicate_based_set(const Predicate&) -> predicate_based_set<Predicate>;

constexpr auto make_set_including_all()
{
    return detail::predicate_based_set
    {
        [](const auto& /*elem*/)
        {
            return true;
        }
    };
}

constexpr auto make_set_excluding_all()
{
    return detail::predicate_based_set
    {
        [](const auto& /*elem*/)
        {
            return false;
        }
    };
}

template<class Elem>
constexpr auto make_set_excluding(const Elem& elem)
{
    return detail::predicate_based_set
    {
        [elem](const auto& elem2)
        {
            return !detail::equals(elem, elem2);
        }
    };
}

template<class Predicate, class Elem>
constexpr bool contains
(
    const predicate_based_set<Predicate>& set,
    const Elem& elem
)
{
    return set.predicate(elem);
}

template<class Predicate, class Elem>
constexpr auto make_set_union
(
    const predicate_based_set<Predicate>& set,
    const Elem& elem
)
{
    return predicate_based_set
    {
        [set, elem](const auto& elem2)
        {
            return contains(set, elem2) || equals(elem, elem2);
        }
    };
}


/*
tuple_based_set

Way less complex to build than `predicate_based_set` for finite list of
elements, because making unions doesn't create deep trees of nested predicates.
*/

template<class... Elems>
struct tuple_based_set
{
    tuple<Elems...> elems;
};

template<class... Elems>
tuple_based_set(const tuple<Elems...>&) -> tuple_based_set<Elems...>;

template<class... Elems>
constexpr auto make_set_including(const Elems&... elems)
{
    return tuple_based_set{make_tuple(elems...)};
}

template<class... Elems, class Elem>
constexpr bool contains
(
    const tuple_based_set<Elems...>& set,
    const Elem& elem
)
{
    return tuple_apply
    (
        set.elems,
        [&elem](const auto&... elems)
        {
            return (equals(elem, elems) || ...);
        }
    );
}

template<class... Elems, class Elem>
constexpr auto make_set_union
(
    const tuple_based_set<Elems...>& set,
    const Elem& elem
)
{
    return tuple_based_set{set.elems.append(elem)};
}


/*
type_list_based_set

Better than `tuple_based_set` for types.
*/

template<class... Ts>
struct type_list_based_set{};

template<class... Ts>
constexpr auto make_set_including_types()
{
    return type_list_based_set<Ts...>{};
}

template<class... Ts, class U>
constexpr bool contains
(
    type_list_based_set<Ts...> /*ignored*/,
    type_t<U> /*ignored*/
)
{
    return tlu::contains_v<type_list_based_set<Ts...>, U>;
}

template<class... Ts, class U>
constexpr auto make_set_union
(
    type_list_based_set<Ts...> /*ignored*/,
    type_t<U> /*ignored*/
)
{
    return tlu::push_back_unique_t
    <
        type_list_based_set<Ts...>,
        U
    >{};
}

template<class... Ts, class... Us>
constexpr auto make_set_union
(
    type_list_based_set<Ts...> /*ignored*/,
    type_list_based_set<Us...> /*ignored*/
)
{
    return tlu::push_back_all_unique_t
    <
        type_list_based_set<Ts...>,
        type_list_based_set<Us...>
    >{};
}


/*
Common

Fall back to `predicate_based_set` when composing sets of different types.
*/

template<class Lhs, class Rhs>
constexpr auto make_set_union(const Lhs& lhs, const Rhs& rhs)
{
    return predicate_based_set
    {
        [lhs, rhs](const auto& elem)
        {
            return contains(lhs, elem) || contains(rhs, elem);
        }
    };
}

template<class Lhs, class Rhs>
constexpr auto make_set_intersection(const Lhs& lhs, const Rhs& rhs)
{
    return predicate_based_set
    {
        [lhs, rhs](const auto& elem)
        {
            return contains(lhs, elem) && contains(rhs, elem);
        }
    };
}

template<class Set>
constexpr auto inverse_set(const Set& set)
{
    return predicate_based_set
    {
        [set](const auto& elem)
        {
            return !contains(set, elem);
        }
    };
}

} //namespace

#endif
