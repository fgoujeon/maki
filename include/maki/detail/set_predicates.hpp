//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_SET_PREDICATES_HPP
#define MAKI_SET_PREDICATES_HPP

#include "tuple.hpp"
#include "equals.hpp"

namespace maki::detail::set_predicates
{

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

} //namespace

#endif
