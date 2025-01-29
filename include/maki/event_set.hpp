//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_EVENT_SET_HPP
#define MAKI_EVENT_SET_HPP

#include "null.hpp"
#include "event.hpp"
#include "detail/set.hpp"
#include "detail/equals.hpp"

namespace maki
{

/**
@brief Represents an @ref event-set "event type set".
*/
template<class Impl>
class event_set;

namespace detail
{
    template<class Impl>
    constexpr auto make_event_set_from_impl(const Impl& impl)
    {
        return event_set<Impl>{impl};
    }

    template<class Impl>
    constexpr const auto& impl(const event_set<Impl>& evt_set)
    {
        return evt_set.impl_;
    }
}

template<class Impl>
class event_set
{
public:
    /**
    @brief Constructs a set that only contains `Event`.
    */
    template<class Event>
    constexpr explicit event_set(const event_t<Event> evt):
        impl_{detail::make_set_including(evt)}
    {
    }

    /**
    @brief Checks whether the set contains `Event`.
    */
    template<class Event>
    [[nodiscard]]
    constexpr bool contains(event_t<Event> /*ignored*/ = {}) const
    {
        return detail::contains(impl_, event<Event>);
    }

private:
#ifndef MAKI_DETAIL_DOXYGEN
    template<class Impl2>
    friend constexpr auto detail::make_event_set_from_impl(const Impl2&);

    template<class Impl2>
    friend constexpr const auto& detail::impl(const event_set<Impl2>&);
#endif

    constexpr explicit event_set(const Impl& impl):
        impl_(impl)
    {
    }

    Impl impl_;
};

/**
@relates event_set
@brief Class template argument deduction guide for `maki::event_set`.
*/
template<class Event>
event_set(event_t<Event>) -> event_set
<
#ifdef MAKI_DETAIL_DOXYGEN
    IMPLEMENTATION_DETAIL
#else
    detail::tuple_based_set<event_t<Event>>
#endif
>;

#ifdef MAKI_DETAIL_DOXYGEN
/**
@relates event_set
@brief An infinite `maki::event_set` that contains all the event types.
*/
inline constexpr auto all_events = IMPLEMENTATION_DETAIL;
#else
inline constexpr auto all_events = detail::make_event_set_from_impl
(
    detail::make_set_including_all()
);
#endif

#ifdef MAKI_DETAIL_DOXYGEN
/**
@relates event_set
@brief An empty `maki::event_set`.
*/
inline constexpr auto no_event = IMPLEMENTATION_DETAIL;
#else
inline constexpr auto no_event = detail::make_event_set_from_impl
(
    detail::make_set_excluding_all()
);
#endif

/**
@relates event_set
@brief Creates a `maki::event_set` that contains all the event types that are
not contained in `evt_set`.
*/
template<class Predicate>
constexpr auto operator!(const event_set<Predicate>& evt_set)
{
    return detail::make_event_set_from_impl
    (
        detail::inverse_set(detail::impl(evt_set))
    );
}

/**
@relates event_set
@brief Creates a `maki::event_set` that contains all the event types but
`Event`.
*/
template<class Event>
constexpr auto operator!(event_t<Event> evt)
{
    return detail::make_event_set_from_impl
    (
        detail::make_set_excluding(evt)
    );
}

/**
@relates event_set
@brief Creates a `maki::event_set` that is the result of the union of `lhs` and
`rhs`.
*/
template<class LhsImpl, class RhsImpl>
constexpr auto operator||
(
    const event_set<LhsImpl>& lhs,
    const event_set<RhsImpl>& rhs
)
{
    return detail::make_event_set_from_impl
    (
        detail::make_set_union(detail::impl(lhs), detail::impl(rhs))
    );
}

/**
@relates event_set
@brief Creates a `maki::event_set` that contains the content of `evt_set`, plus
`Event`.
*/
template<class EventSetPredicate, class Event>
constexpr auto operator||
(
    const event_set<EventSetPredicate>& evt_set,
    const event_t<Event> evt
)
{
    return detail::make_event_set_from_impl
    (
        detail::make_set_union(detail::impl(evt_set), evt)
    );
}

/**
@relates event_set
@brief Creates a `maki::event_set` that contains the content of `evt_set`, plus
`Event`.
*/
template<class Event, class EventSetPredicate>
constexpr auto operator||
(
    const event_t<Event>& evt,
    const event_set<EventSetPredicate>& evt_set
)
{
    return evt_set || evt;
}

/**
@relates event_set
@brief Creates a `maki::event_set` that contains `LhsEvent` and `RhsEvent`.
*/
template<class LhsEvent, class RhsEvent>
constexpr auto operator||
(
    const event_t<LhsEvent>& lhs,
    const event_t<RhsEvent>& rhs
)
{
    return detail::make_event_set_from_impl
    (
        detail::make_set_including(lhs, rhs)
    );
}

/**
@relates event_set
@brief Creates a `maki::event_set` that is the result of the intersection of
`lhs` and `rhs`.
*/
template<class LhsImpl, class RhsImpl>
constexpr auto operator&&
(
    const event_set<LhsImpl>& lhs,
    const event_set<RhsImpl>& rhs
)
{
    return detail::make_event_set_from_impl
    (
        detail::make_set_intersection(detail::impl(lhs) && detail::impl(rhs))
    );
}

namespace detail
{
    template<class Event, class Event2>
    constexpr bool contained_in(const event_t<Event>& lhs, const event_t<Event2>& rhs)
    {
        return equals(lhs, rhs);
    }

    template<class Event>
    constexpr bool contained_in(const event_t<Event>& /*lhs*/, null_t /*rhs*/)
    {
        return false;
    }

    template<class Event, class... Predicates>
    constexpr bool contained_in(const event_t<Event>& evt, const event_set<Predicates>&... evt_sets)
    {
        return (evt_sets.contains(evt) || ...);
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
