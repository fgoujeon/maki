# Completion Transition {#completion-transition}

## Definition

A completion transition occurs whenever the source state is said to be *complete*. What determines the completion of a state depends on whether it's simple or composite.

### From a Simple State

A simple state is considered complete right after its entry action is executed.

In the example below, right after a transition to `A` completes, the state machine transitions from `A` to `B`.

@startuml{completion_transition_from_simple_state.png}
[*] --> A
A -> B
hide empty description
@enduml

### From a Composite State

A composite state is considered complete once all its regions entered their final state.

In the example below, whenever every region of `A` enters its final state, the state machine transitions from `A` to `B`.

@startuml{completion_transition_from_composite_state.png}
[*] --> A
A -> B

state A {
    [*] --> A0
    A0 -> [*] : some_event
    --
    [*] --> A1
    A1 -> [*] : some_other_event
}

hide empty description
@enduml

## How to Define a Completion Transition with Maki

Pass `maki::null` in lieu of an event type:

@snippet concepts/transition/src/main.cpp completion-transition

> [!NOTE]
> Completion transitions from composite states are not implemented yet.
