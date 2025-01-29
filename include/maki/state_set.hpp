//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_STATE_SET_HPP
#define MAKI_STATE_SET_HPP

#include "detail/state_conf_fwd.hpp"
#include "detail/set_predicates.hpp"

namespace maki
{

/**
@brief Represents a @ref state-set "state set".
*/
template<class Predicate>
class state_set;

namespace detail
{
    template<class Predicate>
    constexpr auto make_state_set_from_predicate(const Predicate& pred)
    {
        return state_set<Predicate>{pred};
    }

    template<class StateSetPredicate, class StateConfImpl>
    [[nodiscard]]
    constexpr bool contains
    (
        const state_set<StateSetPredicate>& stt_set,
        const state_conf<StateConfImpl>& stt_conf
    )
    {
        return stt_set.predicate_(&stt_conf);
    }
}

template<class Predicate>
class state_set
{
public:
    constexpr state_set(const state_set& other) = default;
    constexpr state_set(state_set&& other) = default;

    ~state_set() = default;

    constexpr state_set& operator=(const state_set& other) = default;
    constexpr state_set& operator=(state_set&& other) = default;

private:
#ifndef MAKI_DETAIL_DOXYGEN
    template<class Predicate2>
    friend constexpr auto detail::make_state_set_from_predicate(const Predicate2&);

    template<class StateSetPredicate, class StateConfImpl>
    friend constexpr bool detail::contains
    (
        const state_set<StateSetPredicate>&,
        const state_conf<StateConfImpl>&
    );
#endif

    constexpr state_set(const Predicate& pred):
        predicate_(pred)
    {
    }

    Predicate predicate_;
};

/**
@relates state_set
@brief An infinite `maki::state_set` that contains all the states.
*/
inline constexpr auto all_states =
#ifdef MAKI_DETAIL_DOXYGEN
    IMPLEMENTATION_DETAIL
#else
    detail::make_state_set_from_predicate(detail::set_predicates::any{})
#endif
;

/**
@relates state_set
@brief An empty `maki::state_set`.
*/
inline constexpr auto no_state =
#ifdef MAKI_DETAIL_DOXYGEN
    IMPLEMENTATION_DETAIL
#else
    detail::make_state_set_from_predicate(detail::set_predicates::none{})
#endif
;

namespace detail
{
    template<class StateConfImpl>
    constexpr auto make_state_set_from_state_conf(const state_conf<StateConfImpl>& stt_conf)
    {
        return make_state_set_from_predicate(set_predicates::exactly{&stt_conf});
    }
}

/**
@relates state_set
@brief Creates a `maki::state_set` that contains all the states that are not
contained in `stt_set`.
*/
template<class Impl>
constexpr auto operator!(const state_set<Impl>& stt_set)
{
    return detail::make_state_set_from_predicate
    (
        [stt_set](const auto pstate_conf)
        {
            return !detail::contains(stt_set, *pstate_conf);
        }
    );
}

/**
@relates state_set
@brief Creates a `maki::state_set` that contains all the states but the ones
created from `stt_conf`.
*/
template<class StateConfImpl>
constexpr auto operator!(const state_conf<StateConfImpl>& stt_conf)
{
    return !detail::make_state_set_from_state_conf(stt_conf);
}

/**
@relates state_set
@brief Creates a `maki::state_set` that is the result of the union of `lhs` and
`rhs`.
*/
template<class LhsPredicate, class RhsPredicate>
constexpr auto operator||
(
    const state_set<LhsPredicate>& lhs,
    const state_set<RhsPredicate>& rhs
)
{
    return detail::make_state_set_from_predicate
    (
        [lhs, rhs](const auto pstate_conf)
        {
            return detail::contains(lhs, *pstate_conf) || detail::contains(rhs, *pstate_conf);
        }
    );
}

/**
@relates state_set
@brief Creates a `maki::state_set` that contains the states of `stt_set`, plus
the ones created from `stt_conf`.
*/
template<class StateSetPredicate, class StateConfImpl>
constexpr auto operator||
(
    const state_set<StateSetPredicate>& stt_set,
    const state_conf<StateConfImpl>& stt_conf
)
{
    return stt_set || detail::make_state_set_from_state_conf(stt_conf);
}

/**
@relates state_set
@brief Creates a `maki::state_set` that contains the states of `stt_set`, plus
the ones created from `stt_conf`.
*/
template<class StateConfImpl, class StateSetPredicate>
constexpr auto operator||
(
    const state_conf<StateConfImpl>& stt_conf,
    const state_set<StateSetPredicate>& stt_set
)
{
    return detail::make_state_set_from_state_conf(stt_conf) || stt_set;
}

/**
@relates state_set
@brief Creates a `maki::state_set` that contains the states created from `lhs`
and `rhs`.
*/
template<class LhsStateConfImpl, class RhsStateConfImpl>
constexpr auto operator||
(
    const state_conf<LhsStateConfImpl>& lhs,
    const state_conf<RhsStateConfImpl>& rhs
)
{
    return
        detail::make_state_set_from_state_conf(lhs) ||
        detail::make_state_set_from_state_conf(rhs)
    ;
}

/**
@relates state_set
@brief Creates a `maki::state_set` that is the result of the intersection of
`lhs` and `rhs`.
*/
template<class LhsPredicate, class RhsPredicate>
constexpr auto operator&&
(
    const state_set<LhsPredicate>& lhs,
    const state_set<RhsPredicate>& rhs
)
{
    return detail::make_state_set_from_predicate
    (
        [lhs, rhs](const auto pstate_conf)
        {
            return detail::contains(lhs, *pstate_conf) && detail::contains(rhs, *pstate_conf);
        }
    );
}

namespace detail
{
    template<class StateConfImpl, class StateConfImpl2>
    constexpr bool contained_in(const state_conf<StateConfImpl>& lhs, const state_conf<StateConfImpl2>* rhs)
    {
        return equals(&lhs, rhs);
    }

    template<class StateConfImpl, class... Predicates>
    constexpr bool contained_in(const state_conf<StateConfImpl>& stt_conf, const state_set<Predicates>&... state_sets)
    {
        return (detail::contains(state_sets, stt_conf) || ...);
    }

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
