//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_EVENT_SET_HPP
#define MAKI_EVENT_SET_HPP

#include "any.hpp"
#include "none.hpp"
#include "null.hpp"
#include "detail/set_predicates.hpp"
#include "detail/equals.hpp"
#include "event.hpp"

namespace maki
{

/**
@brief Represents a set of event types. See @ref filter.
Use the predefined variables (`maki::any`, `maki::any_of`,
`maki::any_but`, `maki::no_event`) and builder functions
(`maki::any_if()`, `maki::any_if_not()`) instead of manually
instantiating this type.
*/
template<class Predicate>
class event_set
{
public:
    constexpr explicit event_set(const Predicate& pred):
        predicate_(pred)
    {
    }

    template<class Event>
    constexpr event_set(event_t<Event> /*ignored*/):
        predicate_(detail::set_predicates::exactly{event<Event>})
    {
    }

    constexpr event_set(any_t /*ignored*/):
        predicate_(detail::set_predicates::any{})
    {
    }

    constexpr event_set(none_t /*ignored*/):
        predicate_(detail::set_predicates::none{})
    {
    }

    template<class Event>
    [[nodiscard]]
    constexpr bool contains(event_t<Event> /*ignored*/ = {}) const
    {
        return predicate_(event<Event>);
    }

private:
    Predicate predicate_;
};

template<class Predicate>
event_set(const Predicate&) -> event_set<Predicate>;

template<class Event>
event_set(event_t<Event>) -> event_set<detail::set_predicates::exactly<event_t<Event>>>;

event_set(any_t) -> event_set<detail::set_predicates::any>;

event_set(none_t) -> event_set<detail::set_predicates::none>;

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

template<class Event>
constexpr auto operator!(const event_t<Event>& evt)
{
    return !event_set{evt};
}

template<class LhsImpl, class RhsImpl>
constexpr auto operator||(const event_set<LhsImpl>& lhs, const event_set<RhsImpl>& rhs)
{
    return event_set
    {
        [lhs, rhs](const auto evt)
        {
            return lhs.contains(evt) || rhs.contains(evt);
        }
    };
}

template<class LhsEvent, class RhsEvent>
constexpr auto operator||(const event_set<LhsEvent>& lhs, const event_t<RhsEvent>& rhs)
{
    return lhs || event_set{rhs};
}

template<class LhsEvent, class RhsEvent>
constexpr auto operator||(const event_t<LhsEvent>& lhs, const event_set<RhsEvent>& rhs)
{
    return event_set{lhs} || rhs;
}

template<class LhsStateConfImpl, class RhsStateConfImpl>
constexpr auto operator||(const event_t<LhsStateConfImpl>& lhs, const event_t<RhsStateConfImpl>& rhs)
{
    return event_set{lhs} || event_set{rhs};
}

template<class LhsImpl, class RhsImpl>
constexpr auto operator&&(const event_set<LhsImpl>& lhs, const event_set<RhsImpl>& rhs)
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
