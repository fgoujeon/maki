# Self-Transition {#self-transition}

## Definition

ST self-transition is a transition whose source and target is the same state.

### Simple State

In the example below, whenever `ST` is active and the event `E` happens, the state machine executes `exit_action`, then `entry_action`.

@startuml{regular_self_transition_with_simple_state.png}
[*] --> ST
ST -> ST : E
ST : entry / entry_action
ST : exit / exit_action
hide empty description
@enduml

### Composite State

In the case of a composite state, besides the execution of possible exit and entry actions, a self-transition also causes external transitions to the initial state for all the regions.

In the example below, whenever `ST` is active and the event `E` happens, the state machine executes, in the region of `ST`, an external transition from the active substate (whatever it is) to `AA`.

@startuml{regular_self_transition_with_composite_state.png}
[*] --> ST
ST -> ST : E

state ST {
    [*] --> AA
    AA -> AB : some_event
}

hide empty description
@enduml

## How to Define Such a Transition with Maki

An external self-transition is a transition going from `ST` to `ST`:

@snippet concepts/transition/src/main.cpp external-self-transition
