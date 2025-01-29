//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_SET_HPP
#define MAKI_SET_HPP

#include "tuple.hpp"
#include "equals.hpp"

namespace maki::detail
{

namespace set_predicates
{
    template<class T>
    struct exactly
    {
        template<class Value>
        constexpr bool operator()(const Value& other) const
        {
            return equals(value, other);
        }

        T value;
    };

    template<class T>
    exactly(const T& value) -> exactly<T>;

    template<class T>
    struct not_
    {
        template<class Value>
        constexpr bool operator()(const Value& other) const
        {
            return !equals(value, other);
        }

        T value;
    };

    template<class T>
    not_(const T& value) -> not_<T>;

    struct any
    {
        template<class Value>
        constexpr bool operator()(const Value& /*value*/) const
        {
            return true;
        }
    };

    struct none
    {
        template<class Value>
        constexpr bool operator()(const Value& /*value*/) const
        {
            return false;
        }
    };

    template<class... Ts>
    struct any_of
    {
        template<class Value>
        constexpr bool operator()(const Value& value) const
        {
            return tuple_apply
            (
                values,
                [value](const auto&... values)
                {
                    return (equals(values, value) || ...);
                }
            );
        }

        tuple<Ts...> values;
    };

    template<class... Ts>
    struct any_but
    {
        template<class Value>
        constexpr bool operator()(const Value& value) const
        {
            return tuple_apply
            (
                values,
                [value](const auto&... values)
                {
                    return (!equals(values, value) && ...);
                }
            );
        }

        tuple<Ts...> values;
    };
}


/*
tuple_based_set
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
    return tuple_based_set{make_tuple(distributed_construct, elems...)};
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
    return tuple_based_set{tuple_append(set.elems, elem)};
}


/*
predicate_based_set
*/

template<class Predicate>
struct predicate_based_set
{
    Predicate predicate;
};

template<class Predicate>
predicate_based_set(const Predicate&) -> predicate_based_set<Predicate>;

template<class T>
constexpr auto make_set_excluding(const T& elem)
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
common
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
