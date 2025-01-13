//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_EVENT_SET_HPP
#define MAKI_EVENT_SET_HPP

#include "detail/tuple.hpp"
#include "detail/equals.hpp"
#include "event.hpp"

namespace maki
{

namespace detail::event_set_predicates
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
}

/**
@brief Represents a set of event types. See @ref filter.
Use the predefined variables (`maki::any_event`, `maki::any_event_of`,
`maki::any_event_but`, `maki::no_event`) and builder functions
(`maki::any_event_if()`, `maki::any_event_if_not()`) instead of manually
instantiating this type.
*/
template<class Predicate>
struct event_set
{
    Predicate predicate;
};

template<class Predicate>
event_set(const Predicate&) -> event_set<Predicate>;

/**
@brief A set that contains all event types.
*/
#ifdef MAKI_DETAIL_DOXYGEN
constexpr auto any_event = event_set{IMPLEMENTATION_DETAIL};
#else
inline constexpr auto any_event = event_set{detail::event_set_predicates::any{}};
#endif

/**
@brief Makes a set that contains the event types that verifies
`pred(event_type) == true`.
@tparam Predicate the predicate against which event_types are tested
*/
template<class Predicate>
constexpr auto any_event_if(const Predicate& pred)
{
    return event_set{pred};
}

/**
@brief Makes a set that contains the event types that verifies
`pred(event_type) == false`.
@tparam Predicate the predicate against which event_types are tested
*/
template<class Predicate>
constexpr auto any_event_if_not(const Predicate& pred)
{
    return event_set
    {
        [pred](const auto& value)
        {
            !pred(value);
        }
    };
}

/**
@brief A set that contains the given event types.
@tparam Events the event types
*/
template<class... Events>
constexpr auto any_event_of = event_set
{
    detail::event_set_predicates::any_of<event_t<Events>...>
    {
        detail::tuple<event_t<Events>...>
        {
            detail::distributed_construct,
            event<Events>...
        }
    }
};

/**
@brief A set that contains all the event types that are not in the given list.
@tparam Events the list of excluded event types
*/
template<class... Events>
constexpr auto any_event_but = event_set
{
    detail::event_set_predicates::any_but<event_t<Events>...>
    {
        detail::tuple<event_t<Events>...>
        {
            detail::distributed_construct,
            event<Events>...
        }
    }
};

/**
@brief An empty event type set.
*/
#ifdef MAKI_DETAIL_DOXYGEN
constexpr auto no_event = event_set{IMPLEMENTATION_DETAIL};
#else
inline constexpr auto no_event = event_set{detail::event_set_predicates::none{}};
#endif

namespace detail
{
    template<class Lhs, class Rhs>
    constexpr bool matches_filter(const Lhs& lhs, const Rhs& rhs)
    {
        return equals(lhs, rhs);
    }

    template<class Value, class FilterPredicate>
    constexpr bool matches_filter(const Value& value, const event_set<FilterPredicate>& flt)
    {
        return flt.predicate(value);
    }

    template<class Value, class FilterPredicate>
    constexpr bool matches_filter_ptr(const Value& value, const event_set<FilterPredicate>* pflt)
    {
        return pflt->predicate(value);
    }

    template<class Value, class... Filters>
    constexpr bool matches_any_filter(const Value& value, const Filters&... filters)
    {
        return (matches_filter(value, filters) || ...);
    }

    template<class T>
    struct is_event_set
    {
        static constexpr auto value = false;
    };

    template<class Predicate>
    struct is_event_set<event_set<Predicate>>
    {
        static constexpr auto value = true;
    };

    template<class T>
    constexpr auto is_event_set_v = is_event_set<T>::value;
}

} //namespace

#endif
