# Composite State {#composite-state}

## Definition

A composite state is a state that contains its own set of nested states and transitions. Like a state machine, a composite state has exactly one active state (unless it's made of several [regions](@ref region)).

In example below, `running` is a composite state:

@startuml{composite_state.png}
state running {
    [*] --> initializing
    initializing --> idle : end_of_init
    idle -> processing_command : command_request
    processing_command -> idle : command_execution
}

[*] --> running
running -> failing : error
hide empty description
@enduml

Whenever a state machine enters a composite state, it also enters the initial state (`initializing` in our example) of that composite state. Whenever a state machine exits a composite state, it also exits the active state of that composite state.

A state that belongs to a composite state is called a **substate**. A substate can also be composite, turning a state machine into a tree of states.

Whenever a substate is active, all its parent states, or **superstates**, are considered active as well.

## When to Use a Composite State

The need of a composite state can arise from two distinct situations:

* A state implements a subsystem of its own that is complex enough to require organizing; i.e. for the exact same reasons you used a state machine for your whole system in the first place.
* A set of states can be logically grouped and share transitions to the same other states.

For example, an `initializing` state could be turned into a composite state if it manages the initialization of several components, each of which could be handled by a substate.

Another example: a set of states could be grouped into a composite state `running` if they all transition to a `failing` state whenever an error occurs.

## How to Define and Use a Composite State with Maki

You make Maki create a composite state just like you make it create a simple state: by defining a `maki::state_mold`. A state becomes composite as soon as you give it at least one transition table, which you do by calling `maki::state_mold::transition_tables()`.

> [!important]
> When the same state mold is referenced in several transition tables (e.g. the table of a composite state and the table of the state machine root), it's used to make several, independent `maki::state` objects.
