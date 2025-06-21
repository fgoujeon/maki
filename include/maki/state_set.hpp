//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_STATE_SET_HPP
#define MAKI_STATE_SET_HPP

#include "detail/state_mold_fwd.hpp"
#include "detail/impl.hpp"
#include "detail/set.hpp"

namespace maki
{

/**
@brief Represents a @ref state-set "state set".
@tparam Impl implementation detail
*/
template<class Impl>
class state_set;

namespace detail
{
    template<class Impl>
    constexpr auto make_state_set_from_impl(const Impl& impl)
    {
        return state_set<Impl>{impl};
    }
}

template<class Impl>
class state_set
{
public:
    constexpr state_set(const state_set& other) = default;
    constexpr state_set(state_set&& other) = default;

    ~state_set() = default;

    constexpr state_set& operator=(const state_set& other) = default;
    constexpr state_set& operator=(state_set&& other) = default;

private:
    using impl_type = Impl;

    template<class Impl2>
    friend constexpr auto detail::make_state_set_from_impl(const Impl2&);

    explicit constexpr state_set(const Impl& impl):
        impl_(impl)
    {
    }

    MAKI_DETAIL_FRIENDLY_IMPL
};

#ifdef MAKI_DETAIL_DOXYGEN
/**
@relates state_set
@brief An infinite `maki::state_set` that contains all the states.
*/
inline constexpr auto all_states = IMPLEMENTATION_DETAIL;
#else
inline constexpr auto all_states = detail::make_state_set_from_impl
(
    detail::make_set_including_all()
);
#endif

#ifdef MAKI_DETAIL_DOXYGEN
/**
@relates state_set
@brief An empty `maki::state_set`.
*/
inline constexpr auto no_state = IMPLEMENTATION_DETAIL;
#else
inline constexpr auto no_state = detail::make_state_set_from_impl
(
    detail::make_set_excluding_all()
);
#endif

/**
@relates state_set
@brief Creates a `maki::state_set` that contains all the states that are not
contained in `stt_set`.
*/
template<class Impl>
constexpr auto operator!(const state_set<Impl>& stt_set)
{
    return detail::make_state_set_from_impl
    (
        detail::inverse_set(detail::impl_of(stt_set))
    );
}

/**
@relates state_set
@brief Creates a `maki::state_set` that contains all the states but the ones
created by `stt_mold`.
*/
template<class StateMoldImpl>
constexpr auto operator!(const state_mold<StateMoldImpl>& stt_mold)
{
    return detail::make_state_set_from_impl
    (
        detail::make_set_excluding(&stt_mold)
    );
}

/**
@relates state_set
@brief Creates a `maki::state_set` that is the result of the union of `lhs` and
`rhs`.
*/
template<class LhsImpl, class RhsImpl>
constexpr auto operator||
(
    const state_set<LhsImpl>& lhs,
    const state_set<RhsImpl>& rhs
)
{
    return detail::make_state_set_from_impl
    (
        detail::make_set_union(detail::impl_of(lhs), detail::impl_of(rhs))
    );
}

/**
@relates state_set
@brief Creates a `maki::state_set` that contains the states of `stt_set`, plus
the ones created by `stt_mold`.
*/
template<class StateSetImpl, class StateMoldImpl>
constexpr auto operator||
(
    const state_set<StateSetImpl>& stt_set,
    const state_mold<StateMoldImpl>& stt_mold
)
{
    return detail::make_state_set_from_impl
    (
        detail::make_set_union(detail::impl_of(stt_set), &stt_mold)
    );
}

/**
@relates state_set
@brief Creates a `maki::state_set` that contains the states of `stt_set`, plus
the ones created by `stt_mold`.
*/
template<class StateMoldImpl, class StateSetImpl>
constexpr auto operator||
(
    const state_mold<StateMoldImpl>& stt_mold,
    const state_set<StateSetImpl>& stt_set
)
{
    return stt_set || stt_mold;
}

/**
@relates state_set
@brief Creates a `maki::state_set` that contains the states created from `lhs`
and `rhs`.
*/
template<class LhsStateMoldImpl, class RhsStateMoldImpl>
constexpr auto operator||
(
    const state_mold<LhsStateMoldImpl>& lhs,
    const state_mold<RhsStateMoldImpl>& rhs
)
{
    return detail::make_state_set_from_impl
    (
        detail::make_set_including(&lhs, &rhs)
    );
}

/**
@relates state_set
@brief Creates a `maki::state_set` that is the result of the intersection of
`lhs` and `rhs`.
*/
template<class LhsImpl, class RhsImpl>
constexpr auto operator&&
(
    const state_set<LhsImpl>& lhs,
    const state_set<RhsImpl>& rhs
)
{
    return detail::make_state_set_from_impl
    (
        detail::make_set_intersection(detail::impl_of(lhs) && detail::impl_of(rhs))
    );
}

namespace detail
{
    template<class StateMoldImpl, class StateMoldImpl2>
    constexpr bool contained_in(const state_mold<StateMoldImpl>& lhs, const state_mold<StateMoldImpl2>* rhs)
    {
        return equals(&lhs, rhs);
    }

    template<class StateMoldImpl, class... Predicates>
    constexpr bool contained_in(const state_mold<StateMoldImpl>& stt_mold, const state_set<Predicates>&... state_sets)
    {
        return (contains(impl_of(state_sets), &stt_mold) || ...);
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
