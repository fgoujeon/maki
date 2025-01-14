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

#include "state_conf_fwd.hpp"
#include "any.hpp"
#include "none.hpp"
#include "action.hpp"
#include "guard.hpp"
#include "event_set.hpp"
#include "state_set.hpp"
#include "null.hpp"
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
        class SourceStateConfFilter,
        class EventFilter,
        class TargetStateConf,
        action_signature ActionSignature,
        class ActionCallable,
        guard_signature GuardSignature,
        class GuardCallable
    >
    struct transition
    {
        SourceStateConfFilter source_state_conf_filter;
        EventFilter event_filter;
        TargetStateConf target_state_conf;
        action<ActionSignature, ActionCallable> act;
        guard<GuardSignature, GuardCallable> grd;
    };

    template
    <
        class SourceStateConfFilter,
        class EventFilter,
        class TargetStateConf,
        action_signature ActionSignature,
        class ActionCallable,
        guard_signature GuardSignature,
        class GuardCallable
    >
    transition
    (
        SourceStateConfFilter,
        EventFilter,
        TargetStateConf,
        action<ActionSignature, ActionCallable>,
        guard<GuardSignature, GuardCallable>
    ) -> transition
    <
        SourceStateConfFilter,
        EventFilter,
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

    template<class StateConfImpl>
    constexpr auto to_tt_source_state(const state_conf<StateConfImpl>& stt_conf)
    {
        return &stt_conf;
    }

    template<class StateSetImpl>
    constexpr const auto& to_tt_source_state(const state_set<StateSetImpl>& stt_set)
    {
        return stt_set;
    }

    constexpr auto to_tt_source_state(const any_t& /*any*/)
    {
        return state_set{any};
    }

    constexpr auto to_tt_source_state(const none_t& /*none*/)
    {
        return state_set{none};
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

    @param source_state_conf_filter the configuration of the active state (or states, plural, if it's a @ref filter "filter") from which the transition can occur
    @param event_filter the event type (or types, plural, if it's a @ref filter "filter") that can cause the transition to occur
    @param target_state_conf the configuration of the state that becomes active after the transition occurs
    @param action the `maki::action` invoked when the transition occurs, or `maki::null`
    @param guard the `maki::guard` that must return `true` for the transition to occur, or `maki::null`
    */
    template
    <
        class SourceStateConfFilter,
        class EventFilter,
        class TargetStateConfOrNull,
        class ActionOrNull = null_t,
        class GuardOrNull = null_t
    >
    constexpr auto operator()
    (
        const SourceStateConfFilter& source_state_conf_filter,
        const EventFilter& event_filter,
        const TargetStateConfOrNull& target_state_conf,
        const ActionOrNull& action = null,
        const GuardOrNull& guard = null
    )
    {
        constexpr auto valid_1st_arg =
            detail::is_state_conf_v<SourceStateConfFilter> ||
            detail::is_state_set_v<SourceStateConfFilter> ||
            detail::is_any_v<SourceStateConfFilter> ||
            detail::is_none_v<SourceStateConfFilter>
        ;
        static_assert
        (
            valid_1st_arg,
            "1st argument must be an instance of `maki::state_conf` or an object convertible to `maki::state_set`"
        );

        static_assert
        (
            detail::is_event_v<EventFilter> || detail::is_event_set_v<EventFilter> || detail::is_null_v<EventFilter>,
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
                    detail::to_tt_source_state(source_state_conf_filter),
                    event_filter,
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
