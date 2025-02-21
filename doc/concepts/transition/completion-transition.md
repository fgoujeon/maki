# Completion Transition {#completion-transition}

## Definition

A completion transition occurs whenever the source state is said to be *complete*. What determines the completion of a state depends on whether it's simple or composite.

### From a Simple State

A simple state is considered complete right after its entry action is executed.

In the example below, right after a transition to `S` completes, the state machine transitions from `S` to `T`.

@startuml{completion_transition_from_simple_state.png}
[*] --> S
S -> T
hide empty description
@enduml

### From a Composite State

A composite state is considered complete right after all its regions enter their final state.

In the example below, whenever every region of `S` enters its final state, the state machine transitions from `S` to `T`.

@startuml{completion_transition_from_composite_state.png}
[*] --> S
S -> T

state S {
    [*] --> X0
    X0 -> [*] : some_event
    --
    [*] --> X1
    X1 -> [*] : some_other_event
}

hide empty description
@enduml

## How to Define a Completion Transition with Maki

Pass `maki::null` in lieu of an event type:

@snippet concepts/transition/src/main.cpp completion-transition

> [!NOTE]
> Completion transitions from composite states are not implemented yet.
