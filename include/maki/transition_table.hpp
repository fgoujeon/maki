//Copyright Florian Goujeon 2021 - 2025.
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

#include "action.hpp"
#include "guard.hpp"
#include "event_set.hpp"
#include "state_set.hpp"
#include "null.hpp"
#include "detail/state_conf_fwd.hpp"
#include "detail/state_id.hpp"
#include "detail/tuple.hpp"

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
        class SourceStateConf,
        class EventSet,
        class TargetStateConf,
        action_signature ActionSignature,
        class ActionCallable,
        guard_signature GuardSignature,
        class GuardCallable
    >
    struct transition
    {
        SourceStateConf source_state_conf;
        EventSet evt_set;
        TargetStateConf target_state_conf;
        action<ActionSignature, ActionCallable> act;
        guard<GuardSignature, GuardCallable> grd;
    };

    template
    <
        class SourceStateConf,
        class EventSet,
        class TargetStateConf,
        action_signature ActionSignature,
        class ActionCallable,
        guard_signature GuardSignature,
        class GuardCallable
    >
    transition
    (
        SourceStateConf,
        EventSet,
        TargetStateConf,
        action<ActionSignature, ActionCallable>,
        guard<GuardSignature, GuardCallable>
    ) -> transition
    <
        SourceStateConf,
        EventSet,
        TargetStateConf,
        ActionSignature,
        ActionCallable,
        GuardSignature,
        GuardCallable
    >;

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

    @param source_state_conf the configuration of the active state (or states, plural, if it's a @ref state-set "state set") from which the transition can occur
    @param evt_set the event type (or types, plural, if it's an @ref event-set "event type set") that can cause the transition to occur
    @param target_state_conf the configuration of the state that becomes active after the transition occurs
    @param action the `maki::action` invoked when the transition occurs, or `maki::null`
    @param guard the `maki::guard` that must return `true` for the transition to occur, or `maki::null`
    */
    template
    <
        class SourceStateConf,
        class EventSet,
        class TargetStateConfOrNull,
        class ActionOrNull = null_t,
        class GuardOrNull = null_t
    >
    constexpr auto operator()
    (
        const SourceStateConf& source_state_conf,
        const EventSet& evt_set,
        const TargetStateConfOrNull& target_state_conf,
        const ActionOrNull& action = null,
        const GuardOrNull& guard = null
    )
    {
        //If first transition
        if constexpr(sizeof...(Transitions) == 0)
        {
            static_assert
            (
                detail::is_state_conf_v<SourceStateConf>,
                "Source state of first transition can't be a `maki::state_set`"
            );
        }

        static_assert
        (
            detail::is_state_conf_v<SourceStateConf> || detail::is_state_set_v<SourceStateConf>,
            "1st argument must be an instance of `maki::state_conf` or an instance of `maki::state_set`"
        );

        static_assert
        (
            detail::is_event_v<EventSet> || detail::is_event_set_v<EventSet> || detail::is_null_v<EventSet>,
            "2nd argument must be an instance of `maki::event_t`, an instance of `maki::event_set`, or `maki::null`"
        );

        static_assert
        (
            detail::is_state_conf_v<TargetStateConfOrNull> || detail::is_null_v<TargetStateConfOrNull>,
            "3rd argument must be an instance of `maki::state_conf` or `maki::null`."
        );

        static_assert
        (
            detail::is_action_v<ActionOrNull> || detail::is_null_v<ActionOrNull>,
            "4th argument must be an instance of `maki::action` or `maki::null`."
        );

        static_assert
        (
            detail::is_guard_v<GuardOrNull> || detail::is_null_v<GuardOrNull>,
            "5th argument must be an instance of `maki::guard` or `maki::null`."
        );

        return detail::make_transition_table
        (
            tuple_append
            (
                transitions_,
                detail::transition
                {
                    detail::try_making_state_id(source_state_conf),
                    evt_set,
                    detail::try_making_state_id(target_state_conf),
                    detail::to_action(action),
                    detail::to_guard(guard)
                }
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
