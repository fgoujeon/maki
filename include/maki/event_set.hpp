//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_EVENT_SET_HPP
#define MAKI_EVENT_SET_HPP

#include "null.hpp"
#include "detail/set_predicates.hpp"
#include "detail/equals.hpp"
#include "event.hpp"

namespace maki
{

/**
@brief Represents an @ref event-set "event type set".
*/
template<class Predicate>
class event_set
{
public:
    /**
    @brief Constructs a set from a callable that takes a `maki::event_t` and
    returns a `bool`.
    */
    constexpr explicit event_set(const Predicate& pred):
        predicate_(pred)
    {
    }

    /**
    @brief Constructs a set that only contains `Event`.
    */
    template<class Event>
    constexpr event_set(event_t<Event> /*ignored*/):
        predicate_(detail::set_predicates::exactly{event<Event>})
    {
    }

    /**
    @brief Checks whether the set contains `Event`.
    */
    template<class Event>
    [[nodiscard]]
    constexpr bool contains(event_t<Event> /*ignored*/ = {}) const
    {
        return predicate_(event<Event>);
    }

private:
    Predicate predicate_;
};

/**
@relates event_set
@brief Class template argument deduction guide for `maki::event_set`.
*/
template<class Predicate>
event_set(const Predicate&) -> event_set<Predicate>;

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
    detail::set_predicates::exactly<event_t<Event>>
#endif
>;

/**
@relates event_set
@brief An infinite `maki::event_set` that contains all the event types.
*/
inline constexpr auto any_event =
#ifdef MAKI_DETAIL_DOXYGEN
    IMPLEMENTATION_DETAIL
#else
    event_set{detail::set_predicates::any{}}
#endif
;

/**
@relates event_set
@brief An empty `maki::event_set`.
*/
inline constexpr auto no_event =
#ifdef MAKI_DETAIL_DOXYGEN
    IMPLEMENTATION_DETAIL
#else
    event_set{detail::set_predicates::none{}}
#endif
;

/**
@relates event_set
@brief Creates a `maki::event_set` that contains all the event types that are
not contained in `evt_set`.
*/
template<class Predicate>
constexpr auto operator!(const event_set<Predicate>& evt_set)
{
    return event_set
    {
        [evt_set](const auto evt)
        {
            return !evt_set.contains(evt);
        }
    };
}

/**
@relates event_set
@brief Creates a `maki::event_set` that contains all the event types but
`Event`.
*/
template<class Event>
constexpr auto operator!(const event_t<Event>& evt)
{
    return !event_set{evt};
}

/**
@relates event_set
@brief Creates a `maki::event_set` that is the result of the union of `lhs` and
`rhs`.
*/
template<class LhsPredicate, class RhsPredicate>
constexpr auto operator||
(
    const event_set<LhsPredicate>& lhs,
    const event_set<RhsPredicate>& rhs
)
{
    return event_set
    {
        [lhs, rhs](const auto evt)
        {
            return lhs.contains(evt) || rhs.contains(evt);
        }
    };
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
    const event_t<Event>& evt
)
{
    return evt_set || event_set{evt};
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
    return event_set{evt} || evt_set;
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
    return event_set{lhs} || event_set{rhs};
}

/**
@relates event_set
@brief Creates a `maki::event_set` that is the result of the intersection of
`lhs` and `rhs`.
*/
template<class LhsPredicate, class RhsPredicate>
constexpr auto operator&&
(
    const event_set<LhsPredicate>& lhs,
    const event_set<RhsPredicate>& rhs
)
{
    return event_set
    {
        [lhs, rhs](const auto evt)
        {
            return lhs.contains(evt) && rhs.contains(evt);
        }
    };
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
