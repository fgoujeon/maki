# State Machine {#concepts-state-machine}

## Definition

A state machine (also known as finite-state machine, FSM, or SM) is a model that defines all the possible states of a system, the events that trigger a transition from one state to another, and the actions that are performed when these transitions occur.

A very simple example of such a system is a lamp. It has:

* 2 states: off and on;
* 1 event: a button press;
* 2 actions: turn light on and turn light off.

State machines are often represented by diagrams. Here is the state machine of our lamp, under the form of a UML State Diagram:

@startuml{lamp.png} "The state machine of a lamp"
[*] --> off
hide empty description
off -> on : button press / turn light on
on -> off : button press / turn light off
@enduml

The diagram shows that:

* the initial state (i.e. the active state when the machine starts) is `off` (as pointed to by the black dot);
* pressing the button while the `off` state is active executes the `turn light on` action and activates the `on` state;
* pressing the button while the `on` state is active executes the `turn light off` action and activates the `off` state.

## How they can help you

[TODO]

## How to use them in AweSM

[TODO]
