# Region {#region}

## Definition

A region is an independent set of states and transitions, with its own active state.

A set of orthogonal regions is basically as a set of simple state machines running concurrently, but sharing a common part. This common part includes, for example, the [run-to-completion](@ref run-to-completion) queue.

## When to Use Orthogonal Regions

You use orthogonal regions to split the behavior of a system into as many independent parts.

## How to Use Orthogonal Regions With Maki

A state machine is made of one or more regions. To add a region to a state machine, you just have to add a transition table to the list given to `maki::machine_conf::transition_tables()`.
