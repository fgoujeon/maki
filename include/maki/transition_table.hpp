//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::transition and maki::transition_table struct templates
*/

#ifndef MAKI_TRANSITION_TABLE_HPP
#define MAKI_TRANSITION_TABLE_HPP

#include "detail/state_id.hpp"
#include "detail/storable_function.hpp"
#include "detail/tuple.hpp"
#include "null.hpp"

namespace maki
{

/**
@brief Represents a transition table.

A transition table lists all the possible transitions from a state (the source
state) to another (the target state) in a region of a state machine.

To define a transition table, you have to instantiate an empty
`maki::transition_table` and call
`maki::transition_table::operator()()` for each transition, like so:

```cpp
constexpr auto transition_table = maki::transition_table{}
    //source state, event,                    target state, action,        guard
    (off,           maki::type<button_press>, on,           turn_light_on, has_enough_power)
    (on,            maki::type<button_press>, off,          turn_light_off)
;
```
*/
template<class... Transitions>
class transition_table;

namespace detail
{
    template
    <
        class SourceStateConfFilter,
        class EventFilter,
        class TargetStateConf,
        class Action,
        class Guard
    >
    struct transition
    {
        SourceStateConfFilter source_state_conf_filter;
        EventFilter event_filter;
        TargetStateConf target_state_conf;
        Action action;
        Guard guard;
    };

    template
    <
        class SourceStateConfFilter,
        class EventFilter,
        class TargetStateConf,
        class Action,
        class Guard
    >
    constexpr auto make_transition
    (
        const SourceStateConfFilter& source_state_conf_filter,
        const EventFilter& event_filter,
        const TargetStateConf& target_state_conf,
        const Action& action,
        const Guard& guard
    )
    {
        return transition
        <
            SourceStateConfFilter,
            EventFilter,
            TargetStateConf,
            storable_function_t<Action>,
            storable_function_t<Guard>
        >
        {
            source_state_conf_filter,
            event_filter,
            target_state_conf,
            action,
            guard
        };
    }

    template<class... Transitions>
    constexpr auto make_transition_table(const tuple<Transitions...>& transitions)
    {
        return transition_table<Transitions...>{transitions};
    }

    template<class... Transitions>
    constexpr const auto& rows(const transition_table<Transitions...>& table)
    {
        return table.transitions_;
    }
}

template<class... Transitions>
class transition_table
{
public:
    /**
    @brief Default constructor
    */
    constexpr transition_table() = default;

    /**
    @brief Copy constructor
    */
    constexpr transition_table(const transition_table&) = default;

    /**
    @brief Deleted move constructor
    */
    transition_table(transition_table&&) = delete;

    ~transition_table() = default;

    /**
    @brief Deleted assignment operator
    */
    transition_table& operator=(const transition_table&) = delete;

    /**
    @brief Deleted move operator
    */
    transition_table& operator=(transition_table&&) = delete;

    /**
    @brief Creates a new `transition_table` with an additional transition.

    @param source_state_conf_filter the configurator of the active state (or states, plural, if it's a @ref filter "filter") from which the transition can occur
    @param event_filter the event type (or types, plural, if it's a @ref filter "filter") that can cause the transition to occur
    @param target_state_conf the configurator of the state that becomes active after the transition occurs
    @param action the function invoked when the transition occurs
    @param guard the function that must return `true` for the transition to occur
    */
    template
    <
        class SourceStateConfFilter,
        class EventFilter,
        class TargetStateConf,
        class Action = decltype(null),
        class Guard = decltype(null)
    >
    constexpr auto operator()
    (
        const SourceStateConfFilter& source_state_conf_filter,
        const EventFilter& event_filter,
        const TargetStateConf& target_state_conf,
        const Action& action = null,
        const Guard& guard = null
    )
    {
        return detail::make_transition_table
        (
            tuple_append
            (
                transitions_,
                detail::make_transition
                (
                    detail::try_making_state_id(source_state_conf_filter),
                    event_filter,
                    detail::try_making_state_id(target_state_conf),
                    action,
                    guard
                )
            )
        );
    }

private:
#ifndef MAKI_DETAIL_DOXYGEN
    template<class... Transitions2>
    friend constexpr auto detail::make_transition_table(const detail::tuple<Transitions2...>&);

    template<class... Transitions2>
    friend constexpr const auto& detail::rows(const transition_table<Transitions2...>&);
#endif

    template<class TransitionTuple>
    constexpr explicit transition_table(const TransitionTuple& transitions):
        transitions_{transitions}
    {
    }

    detail::tuple<Transitions...> transitions_;
};

} //namespace

#endif
