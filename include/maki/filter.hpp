//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the filter type and builder functions
*/

#ifndef MAKI_FILTER_HPP
#define MAKI_FILTER_HPP

#include "detail/state_id.hpp"
#include "detail/tuple.hpp"
#include "type.hpp"
#include <type_traits>

namespace maki
{

namespace detail
{
    template<class Lhs, class Rhs>
    constexpr bool equals(const Lhs& lhs, const Rhs& rhs)
    {
        if constexpr(std::is_same_v<Lhs, Rhs>)
        {
            return lhs == rhs;
        }
        else
        {
            return false;
        }
    }

    namespace filter_predicates
    {
        template<class T>
        struct exactly
        {
            template<class Value>
            constexpr bool operator()(const Value& value) const
            {
                return equals(val, value);
            }

            T val;
        };

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

            detail::tuple<Ts...> values;
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

            detail::tuple<Ts...> values;
        };
    }
}

/**
@brief Represents a filter of values. See @ref filter.
Use the variables (`maki::any`, `maki::none`) and builder functions
(`maki::any_if()`, `maki::any_if_not()`, `maki::any_of()`, `maki::any_but()`)
instead of manually instantiating this type.
*/
template<class Predicate>
struct filter
{
    Predicate predicate;
};

template<class Predicate>
filter(const Predicate&) -> filter<Predicate>;

/**
@brief A filter that matches with any value.
*/
#ifdef MAKI_DETAIL_DOXYGEN
constexpr auto any = filter{IMPLEMENTATION_DETAIL};
#else
inline constexpr auto any = filter{detail::filter_predicates::any{}};
#endif

/**
@brief Makes a filter that matches with any value that verifies
`pred(value) == true`.
@tparam Predicate the predicate against which values are tested
*/
template<class Predicate>
constexpr auto any_if(const Predicate& pred)
{
    return filter{pred};
}

/**
@brief Makes a filter that matches with any value that verifies
`pred(value) == false`.
@tparam Predicate the predicate against which values are tested
*/
template<class Predicate>
constexpr auto any_if_not(const Predicate& pred)
{
    return filter
    {
        [pred](const auto& value)
        {
            !pred(value);
        }
    };
}

/**
@brief Makes a filter that matches with the given values.
@tparam Ts the types of values the filter matches with
*/
template<class... Ts>
constexpr auto any_of(const Ts&... values)
{
    return filter
    {
        detail::filter_predicates::any_of<detail::to_state_id_or_identity_t<Ts>...>
        {
            detail::tuple<detail::to_state_id_or_identity_t<Ts>...>
            {
                detail::distributed_construct,
                detail::try_making_state_id(values)...
            }
        }
    };
}

/**
@brief Makes a filter that matches with any values but the given ones.
@tparam Ts the types of values the filter doesn't match with
*/
template<class... Ts>
constexpr auto any_but(const Ts&... values)
{
    return filter
    {
        detail::filter_predicates::any_but<detail::to_state_id_or_identity_t<Ts>...>
        {
            detail::tuple<detail::to_state_id_or_identity_t<Ts>...>
            {
                detail::distributed_construct,
                detail::try_making_state_id(values)...
            }
        }
    };
}

/**
@brief A filter that doesn't match with any value.
*/
#ifdef MAKI_DETAIL_DOXYGEN
constexpr auto none = filter{IMPLEMENTATION_DETAIL};
#else
inline constexpr auto none = filter{detail::filter_predicates::none{}};
#endif

template<class... Types>
constexpr auto any_type_of = any_of(type<Types>...);

template<class... Types>
constexpr auto any_type_but = any_but(type<Types>...);

namespace detail
{
    template<class Lhs, class Rhs>
    constexpr bool matches_filter(const Lhs& lhs, const Rhs& rhs)
    {
        return equals(lhs, rhs);
    }

    template<class Value, class FilterPredicate>
    constexpr bool matches_filter(const Value& value, const filter<FilterPredicate>& flt)
    {
        return flt.predicate(value);
    }

    template<class Value, class FilterPredicate>
    constexpr bool matches_filter_ptr(const Value& value, const filter<FilterPredicate>* pflt)
    {
        return pflt->predicate(value);
    }

    template<class Value, class... Filters>
    constexpr bool matches_any_filter(const Value& value, const Filters&... filters)
    {
        return (matches_filter(value, filters) || ...);
    }

    template<class T>
    struct is_filter
    {
        static constexpr auto value = false;
    };

    template<class Predicate>
    struct is_filter<filter<Predicate>>
    {
        static constexpr auto value = true;
    };

    template<class T>
    constexpr auto is_filter_v = is_filter<T>::value;
}

} //namespace

#endif
