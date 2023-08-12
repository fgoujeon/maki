# State Machine {#state-machine}

## Definition

A state machine (also known as finite-state machine, FSM, or SM) is a model that defines all the possible states of a system, the events that trigger a transition from one state to another, and the actions that are performed when these transitions occur.

A very simple example of such a system is a lamp. It has:

* 2 states: off and on;
* 1 event: a button press;
* 2 actions: turn light on and turn light off.

State machines are often represented by diagrams. Here is the state machine of our lamp, under the form of a UML state diagram:

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

## When to use a state machine

When suitable, state machines help you:

* improve the readability (and thus maintainability) of your code;
* organize your code (by putting each state in its own file, for example);
* visualize what your code is doing at a high level (particularly if it is backed by a state diagram and/or a transition table);
* log meaningful messages to keep track of the state of your program.

State machines are particularly well suited to implement the event-processing part of an event-driven architecture.

If your code defines many event-handling functions like the following one, it would most likely benefit from the introduction of a state machine:

```cpp
void on_button_press()
{
    if(is_power_on)
    {
        if(motor_is_running)
        {
            //...
        }
        else if(5 < temperature_degc && temperature_degc < 50)
        {
            //...
        }
    }
    else
    {
        //...
    }
}
```

## What Maki can do for you

Admittedly, you don't *need* a library to implement a state machine. A couple of `enum`s and `switch`es can do the trick for simple cases. However, when things get complicated or when you need more advanced features, some sort of state machine framework becomes very handy, if not mandatory.

One of the most important things a library such as Maki provides is a **transition table**.

When you set up a state machine with Maki, you have to write a transition table. A transition table is a direct representation of a state diagram in your code. Here is the transition table of our lamp, written with Maki:

@snippet lamp/src/main.cpp transition-table

This is hopefully self-explanatory, but here is how it is structured:

* each row (i.e. each `maki::transition_table_tpl::add`) of the table represents a possible state transition;
* when `source state` is the active state and `event` occurs, the state machine executes `action` and activates `target state`.

This gives you a nice overview of what a program does. Moreover, the more complex a state machine is, the more valuable a transition table is for the readability of your code.

**[TIP]** Transition tables are much more readable when their "columns" are properly aligned. If an alignment plugin exists for your code editor, it's time to install it!

### Example program

Here is a test program that uses the transition table we defined above:

@snippet lamp/src/main.cpp all

The output of this program is the following:

@include lamp/expected-output.txt
