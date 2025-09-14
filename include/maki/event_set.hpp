//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_EVENT_SET_HPP
#define MAKI_EVENT_SET_HPP

#include "event.hpp"
#include "detail/set.hpp"
#include "detail/impl.hpp"

namespace maki
{

/**
@brief Represents an @ref event-set "event type set".
@tparam Impl implementation detail
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
}

template<class Impl>
class event_set
{
public:
    /**
    @brief Constructs a set that only contains `Event`.
    */
    template<class Event>
    constexpr explicit event_set(event_t<Event> /*evt*/)
    {
    }

    /**
    @brief Checks whether the set contains `Event`.
    */
    template<class Event>
    [[nodiscard]]
    constexpr bool contains(event_t<Event> /*ignored*/ = {}) const
    {
        return detail::contains(impl_, detail::type<Event>);
    }

private:
    using impl_type = Impl;

    template<class Impl2>
    friend constexpr auto detail::make_event_set_from_impl(const Impl2&);

    constexpr explicit event_set(const Impl& impl):
        impl_(impl)
    {
    }

    MAKI_DETAIL_FRIENDLY_IMPL
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
    decltype(detail::make_set_including_types<Event>())
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
        detail::inverse_set(detail::impl_of(evt_set))
    );
}

/**
@relates event_set
@brief Creates a `maki::event_set` that contains all the event types but
`Event`.
*/
template<class Event>
constexpr auto operator!(event_t<Event> /*evt*/)
{
    return detail::make_event_set_from_impl
    (
        detail::make_set_excluding(detail::type<Event>)
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
        detail::make_set_union(detail::impl_of(lhs), detail::impl_of(rhs))
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
    event_t<Event> /*evt*/
)
{
    return detail::make_event_set_from_impl
    (
        detail::make_set_union(detail::impl_of(evt_set), detail::type<Event>)
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
    const event_t<Event> evt,
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
    event_t<LhsEvent> /*lhs*/,
    event_t<RhsEvent> /*rhs*/
)
{
    return detail::make_event_set_from_impl
    (
        detail::make_set_including_types<LhsEvent, RhsEvent>()
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
        detail::make_set_intersection(detail::impl_of(lhs) && detail::impl_of(rhs))
    );
}

namespace detail
{
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
