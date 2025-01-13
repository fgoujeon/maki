//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_STATE_SET_HPP
#define MAKI_STATE_SET_HPP

#include "state_conf_fwd.hpp"
#include "detail/tuple.hpp"
#include "detail/equals.hpp"

namespace maki
{

namespace detail::state_set_predicates
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
@brief Represents a set of states. See @ref filter.
Use the predefined variables (`maki::any_state`, `maki::no_state`) and builder
functions (`maki::any_state_if()`, `maki::any_state_if_not()`,
`maki::any_state_of()`, `maki::any_state_but()`) instead of manually
instantiating this type.
*/
template<class Predicate>
struct state_set
{
    Predicate predicate;
};

template<class Predicate>
state_set(const Predicate&) -> state_set<Predicate>;

/**
@brief A set that contains all states.
*/
#ifdef MAKI_DETAIL_DOXYGEN
constexpr auto any_state = state_set{IMPLEMENTATION_DETAIL};
#else
inline constexpr auto any_state = state_set{detail::state_set_predicates::any{}};
#endif

/**
@brief Makes a set that contains any state that verifies
`pred(state_conf) == true`.
@tparam Predicate the predicate against which state configurations are tested
*/
template<class Predicate>
constexpr auto any_state_if(const Predicate& pred)
{
    return state_set{pred};
}

/**
@brief Makes a set that contains any state that verifies
`pred(state_conf) == false`.
@tparam Predicate the predicate against which state configurations are tested
*/
template<class Predicate>
constexpr auto any_state_if_not(const Predicate& pred)
{
    return state_set
    {
        [pred](const auto& stt_conf)
        {
            !pred(stt_conf);
        }
    };
}

/**
@brief Makes a set that contains all the states created from the given
configurations.
@tparam Ts the types of values the filter matches with
*/
template<class... StateConfImpls>
constexpr auto any_state_of(const state_conf<StateConfImpls>&... state_confs)
{
    return state_set
    {
        detail::state_set_predicates::any_of<const state_conf<StateConfImpls>*...>
        {
            detail::tuple<const state_conf<StateConfImpls>*...>
            {
                detail::distributed_construct,
                &state_confs...
            }
        }
    };
}

/**
@brief Makes a set that contains all the states not created from the given
configurations.
@tparam Ts the types of values the filter matches with
*/
template<class... StateConfImpls>
constexpr auto any_state_but(const state_conf<StateConfImpls>&... state_confs)
{
    return state_set
    {
        detail::state_set_predicates::any_but<const state_conf<StateConfImpls>*...>
        {
            detail::tuple<const state_conf<StateConfImpls>*...>
            {
                detail::distributed_construct,
                &state_confs...
            }
        }
    };
}

/**
@brief An empty state set.
*/
#ifdef MAKI_DETAIL_DOXYGEN
constexpr auto no_state = state_set{IMPLEMENTATION_DETAIL};
#else
inline constexpr auto no_state = state_set{detail::state_set_predicates::none{}};
#endif

namespace detail
{
    template<class T>
    struct is_state_set
    {
        static constexpr auto value = false;
    };

    template<class Predicate>
    struct is_state_set<state_set<Predicate>>
    {
        static constexpr auto value = true;
    };

    template<class T>
    constexpr auto is_state_set_v = is_state_set<T>::value;
}

} //namespace

#endif
