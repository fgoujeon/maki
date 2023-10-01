//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_TRANSITION_TABLE_HPP
#define MAKI_TRANSITION_TABLE_HPP

namespace maki
{

/**
@defgroup TransitionTable Transition Table
@brief These are the types and functions that must be used to define transition tables.

A transition table lists all the possible transitions from a state (the source
state) to another (the target state) in a region of a state machine.

You can define a transition table by using this class template directly:
```cpp
constexpr auto transition_table = maki::transition_table
<
    maki::transition<off, button_press, on,  turn_light_on, has_enough_power>,
    maki::transition<on,  button_press, off, turn_light_off>
>{};
```

… but using the @ref transition_table_c variable template and the
@ref transition_table::add member type template is usually the preferred,
more concise way to do so:
```cpp
constexpr auto transition_table = maki::transition_table_c
    .add<off, button_press, on,  turn_light_on, has_enough_power>
    .add<on,  button_press, off, turn_light_off>
;
```

Note that the first usage may be more appropriate in the context of a template
in order to avoid awkward `typename`s and `template`s.

@{
*/

/**
@brief A null event or target state.

Represents either:
- a null event (for anonymous transitions);
- a null target state (for internal transitions in transition table).
*/
struct null{};

/**
@brief An action that does nothing.
*/
inline constexpr void noop()
{
}

/**
@brief A guard that returns `true`.
*/
inline constexpr bool yes()
{
    return true;
}

/**
@brief Represents a possible state transition.

Used as a template argument of @ref transition_table.

@tparam SourceStatePattern the active state (or states, plural, if it's a @ref TypePatterns "type pattern") from which the transition can occur
@tparam EventPattern the event type (or types, plural, if it's a @ref TypePatterns "type pattern") that can cause the transition to occur
@tparam TargetState the state that becomes active after the transition occurs
@tparam Action the function invoked when the transition occurs
@tparam Guard the function that must return `true` for the transition to occur
*/
template
<
    class SourceStatePattern,
    class EventPattern,
    class TargetState,
    const auto& Action = noop,
    const auto& Guard = yes
>
struct transition
{
    using source_state_type_pattern = SourceStatePattern;
    using event_type_pattern = EventPattern;
    using target_state_type = TargetState;

    static constexpr const auto& action = Action;
    static constexpr const auto& guard = Guard;
};

/**
@brief Represents a transition table.

@tparam Transitions the transitions, which must be instances of @ref transition
*/
template<class... Transitions>
struct transition_table
{
    /**
    A type alias to a @ref transition_table with an appended @ref transition.
    See @ref transition for a description of the template parameters.
    */
    template
    <
        class SourceStatePattern,
        class EventPattern,
        class TargetState,
        const auto& Action = noop,
        const auto& Guard = yes
    >
    static constexpr auto add = transition_table
    <
        Transitions...,
        transition<SourceStatePattern, EventPattern, TargetState, Action, Guard>
    >{};
};

/**
@brief A constexpr instance of an empty transition_table

See @ref transition_table for a usage.
*/
inline constexpr auto transition_table_c = transition_table<>{};

/**
@}
*/

} //namespace

#endif
