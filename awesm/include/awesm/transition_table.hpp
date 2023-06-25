//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_TRANSITION_TABLE_HPP
#define AWESM_TRANSITION_TABLE_HPP

namespace awesm
{

/**
@defgroup TransitionTable Transition Table
@brief These are the types and functions that must be used to define transition tables.

A transition table lists all the possible transitions from a state (the source
state) to another (the target state) in a region of a state machine.

You can define a transition table by using this class template directly:
```cpp
using transition_table_t = awesm::transition_table_tpl
<
    awesm::transition<off, button_press, on,  turn_light_on, has_enough_power>,
    awesm::transition<on,  button_press, off, turn_light_off>
>;
```

… but using the @ref transition_table alias and the
@ref transition_table_tpl::add member type template is usually the preferred,
more concise way to do so:
```cpp
using transition_table_t = awesm::transition_table
    ::add<off, button_press, on,  turn_light_on, has_enough_power>
    ::add<on,  button_press, off, turn_light_off>
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

Used as a template argument of @ref transition_table_tpl.

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
struct transition_table_tpl
{
    /**
    A type alias to a @ref transition_table_tpl with an appended @ref
    transition.
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
    using add = transition_table_tpl
    <
        Transitions...,
        transition<SourceStatePattern, EventPattern, TargetState, Action, Guard>
    >;
};

/**
@brief A handy type alias for defining a transition table.

See @ref transition_table_tpl for a usage.
*/
using transition_table = transition_table_tpl<>;

/**
@}
*/

} //namespace

#endif
