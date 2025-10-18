//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_EVENT_SET_HPP
#define MAKI_EVENT_SET_HPP

#include "event.hpp"
#include "detail/type_set.hpp"
#include "detail/friendly_impl.hpp"

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
    constexpr event_set<Impl> make_event_set_from_impl()
    {
        return event_set<Impl>{};
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
    constexpr bool contains() const
    {
        return detail::type_set_contains_v<Impl, Event>;
    }

    /**
    @brief Checks whether the set contains `Event`.
    */
    template<class Event>
    [[nodiscard]]
    constexpr bool contains(event_t<Event> /*ignored*/) const
    {
        return detail::type_set_contains_v<Impl, Event>;
    }

private:
    using impl_type = Impl;

    template<class T>
    friend struct detail::impl_of_t_helper;

    template<class Impl2>
    friend constexpr event_set<Impl2> detail::make_event_set_from_impl();

    constexpr event_set() = default;
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
    detail::type_set_item<Event>
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
<
    detail::universal_type_set_t
>();
#endif

#ifdef MAKI_DETAIL_DOXYGEN
/**
@relates event_set
@brief An empty `maki::event_set`.
*/
inline constexpr auto no_event = IMPLEMENTATION_DETAIL;
#else
inline constexpr auto no_event = detail::make_event_set_from_impl
<
    detail::empty_type_set_t
>();
#endif

/**
@relates event_set
@brief Creates a `maki::event_set` that contains all the event types that are
not contained in `event_types`.
*/
template<class EventSetImpl>
constexpr auto operator!
(
#ifdef MAKI_DETAIL_DOXYGEN
    const event_set<EventSetImpl>& event_types
#else
    const event_set<EventSetImpl>& /*event_types*/
#endif
)
{
    return detail::make_event_set_from_impl
    <
        detail::type_set_inverse_t<EventSetImpl>
    >();
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
    <
        detail::type_set_exclusion_list<Event>
    >();
}

/**
@relates event_set
@brief Creates a `maki::event_set` that is the result of the union of `lhs` and
`rhs`.
*/
template<class LhsImpl, class RhsImpl>
constexpr auto operator||
(
#ifdef MAKI_DETAIL_DOXYGEN
    const event_set<LhsImpl>& lhs,
    const event_set<RhsImpl>& rhs
#else
    const event_set<LhsImpl>& /*lhs*/,
    const event_set<RhsImpl>& /*rhs*/
#endif
)
{
    return detail::make_event_set_from_impl
    <
        detail::type_set_union_t<LhsImpl, RhsImpl>
    >();
}

/**
@relates event_set
@brief Creates a `maki::event_set` that contains the content of `event_types`,
plus `Event`.
*/
template<class LhsImpl, class Event>
constexpr auto operator||
(
#ifdef MAKI_DETAIL_DOXYGEN
    const event_set<LhsImpl>& lhs,
    event_t<Event> rhs
#else
    const event_set<LhsImpl>& /*lhs*/,
    event_t<Event> /*rhs*/
#endif
)
{
    return detail::make_event_set_from_impl
    <
        detail::type_set_union_t
        <
            LhsImpl,
            detail::type_set_item<Event>
        >
    >();
}

/**
@relates event_set
@brief Creates a `maki::event_set` that contains the content of `event_types`,
plus `Event`.
*/
template<class Event, class RhsImpl>
constexpr auto operator||
(
    const event_t<Event> lhs,
    const event_set<RhsImpl>& rhs
)
{
    return rhs || lhs;
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
    <
        detail::type_set_inclusion_list<LhsEvent, RhsEvent>
    >();
}

/**
@relates event_set
@brief Creates a `maki::event_set` that is the result of the intersection of
`lhs` and `rhs`.
*/
template<class LhsImpl, class RhsImpl>
constexpr auto operator&&
(
#ifdef MAKI_DETAIL_DOXYGEN
    const event_set<LhsImpl>& lhs,
    const event_set<RhsImpl>& rhs
#else
    const event_set<LhsImpl>& /*lhs*/,
    const event_set<RhsImpl>& /*rhs*/
#endif
)
{
    return detail::make_event_set_from_impl
    <
        detail::type_set_intersection_t<LhsImpl, RhsImpl>
    >();
}

namespace detail
{
    template<class T>
    struct is_event_set
    {
        static constexpr auto value = false;
    };

    template<class EventSetImpl>
    struct is_event_set<event_set<EventSetImpl>>
    {
        static constexpr auto value = true;
    };

    template<class T>
    constexpr auto is_event_set_v = is_event_set<T>::value;
}

} //namespace

#endif
