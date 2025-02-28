# Local Transition {#local-transition}

## Definition

A transition is said to be **external** when it exits its source state. Otherwise, the transition is said to be **local**.

Exiting a state causes:

1. (for composite states only) the active states to be exited;
2. the execution of the exit action, if any.

Entering a state causes:

1. the execution of the entry action, if any;
2. (for composite states only) the initial states to be entered.

A transition to a sibling state or to the superstate is inevitably external, as the source state must be exited to allow the target state to be entered. However, in the case of a transition to a substate, or a self-transition, things can be different.

### Case #1: Transition to Substate

An **external** transition from `S` to its substate `T` looks like this:

![](transition_locality_substate_external.png)

In this example, whenever `S` is active and the event `E` occurs, the state machine:

1. exits the active substate of `S`, whatever it is;
2. executes `exit_action`;
3. executes `entry_action`;
4. enters `T`.

Now, let's change this example a bit by making the transition from `S` to `T` **local**:

![](transition_locality_substate_local.png)

In this new example, whenever `S` is active and the event `E` occurs, the state machine:

1. exits the active substate of `S`, whatever it is;
2. enters `T`;

that is, the same steps as with an external transition, except the 2nd and 3rd ones, because `S` is neither exited nor reentered.

A local transition to a substate can be seen as an external transition from *any state of the region* to a target state of the same region. As it happens, the notation below is also valid:

@startuml{transition_locality_substate_local_star.png}
[*] --> S
state S {
    [*] --> X
    state "*" as any
    any -> T : E
}
S : entry / entry_action
S : exit / exit_action
hide empty description
@enduml

Other regions, if any, are not affected.

### Case #2: Self-Transition

An **external** transition from `ST` to itself looks like this:

@startuml{transition_locality_self_external.png}
[*] --> ST
state ST {
    [*] --> X
    X -> Y : some_event
}
ST : entry / entry_action
ST : exit / exit_action
ST -> ST : E / A
hide empty description
@enduml

In this example, whenever `ST` is active and the event `E` occurs, the state machine:

1. exits the active substate of `ST`, whatever it is;
2. executes `exit_action`;
3. executes the action `A`;
4. executes `entry_action`;
5. enters `X`.

Now, let's change this example a bit by making the transition from `ST` to itself **local**:

@startuml{transition_locality_self_local.png}
[*] --> ST
state ST {
    [*] --> X
    X -> Y : some_event
}
ST : entry / entry_action
ST : exit / exit_action
ST : E / A
hide empty description
@enduml

In this new example, whenever `ST` is active and the event `E` occurs, the state machine:

1. executes the action `A`;

that is, only the 3rd step of the equivalent external transition, because `ST` is neither exited nor reentered.

The special case of a local self-transition is called an **internal transition**. The action associated to an internal transition is called an **internal action**.

## How to Define a Local Transition with Maki

Within Maki, all transitions are external by default. You can express local transitions for the two cases we've seen.

### Case #1: Local Transition to Substate

@ref state-set "State sets" extend the concept of local transitions to substates; a transition whose source state is `maki::all_states` is the equivalent of such a transition:

@snippet concepts/transition/src/main.cpp local-transition-to-substate

### Case #2: Internal Transition

In the transition table, you can pass `maki::null` in lieu of a target state to define an internal transition:

@snippet concepts/transition/src/main.cpp internal-transition

You can also implicitly create a guardless internal transition (which would be prepended to the transition table of the parent region) by adding an internal action to the configuration of the state:

@snippet concepts/transition/src/main.cpp internal-transition-in-state-conf
