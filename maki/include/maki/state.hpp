//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_STATE_HPP
#define MAKI_STATE_HPP

#include "noop.hpp"
#include "type.hpp"
#include "detail/tuple_2.hpp"
#include <string_view>

namespace maki
{

template<class OnEntry = noop_t, class OnEvent = detail::tuple_2<>, class OnExit = noop_t>
struct state;

namespace detail
{
    template<class OnEntry = noop_t, class OnEvent = detail::tuple_2<>, class OnExit = noop_t>
    constexpr auto make_state
    (
        const OnEntry& on_entry,
        const OnEvent& on_event,
        const OnExit& on_exit,
        const std::string_view pretty_name
    )
    {
        return state<OnEntry, OnEvent, OnExit>{on_entry, on_event, on_exit, pretty_name};
    }
}

template<class OnEntry, class OnEventTuple, class OnExit>
struct state
{
    OnEntry on_entry = noop;
    OnEventTuple on_events = detail::make_tuple_2(); //Tuple of {event-type, lambda} pairs
    OnExit on_exit = noop;
    std::string_view pretty_name;

    template<class Value>
    [[nodiscard]] constexpr auto set_on_entry(const Value& value) const
    {
        return detail::make_state(value, on_events, on_exit, pretty_name);
    }

    template<class Event, class Value>
    [[nodiscard]] constexpr auto add_on_event
    (
        const type_t<Event> event_type,
        const Value& value
    ) const
    {
        return detail::make_state
        (
            on_entry,
            detail::tuple_util::push_back(on_events, detail::make_tuple_2(event_type, value)),
            on_exit,
            pretty_name
        );
    }

    template<class Event, class Value>
    [[nodiscard]] constexpr auto add_on_event(const Value& value) const
    {
        return add_on_event(type_t<Event>{}, value);
    }

    template<class Value>
    [[nodiscard]] constexpr auto set_on_exit(const Value& value) const
    {
        return detail::make_state(on_entry, on_events, value, pretty_name);
    }

    [[nodiscard]] constexpr auto set_pretty_name(const std::string_view value) const
    {
        return detail::make_state(on_entry, on_events, on_exit, value);
    }
};

inline constexpr auto state_c = state<>{};

} //namespace

#endif
