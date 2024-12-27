# Composite State {#composite-state}

## Definition

A composite state is a state that runs its own, nested state machine.

Like a state machine, a composite state can be made of several [orthogonal regions](@ref orthogonal-region).

A state that belongs to a composite state is called a **substate**.

## When to Use a Composite State

The need of a composite state can arise from two distinct situations:

* A state implements a subsystem of its own that is complex enough to require organizing; i.e. for the exact same reasons you used a state machine for your whole system in the first place.
* A set of states can be logically grouped and share transitions to the same other states.

For example, an `initializing` state could be turned into a composite state if it manages the initialization of several components, each of which could be handled by a substate.

Another example: a set of states could be grouped into a composite state `running` if they all transition to a `failing` state whenever an error occurs.

## How to Define and Use a Composite State with Maki

You make Maki create a composite state just like you make it create a simple state: by defining a `maki::state_conf`. A state becomes composite as soon as you give it a set of transition tables, which you do by calling `maki::state_conf::transition_tables()`. Each transition table leads to the creation of an [orthogonal region](@ref orthogonal-region).

Whenever a state machine enters a composite state, its active substates are the first state of each transition table.
