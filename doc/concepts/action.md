# Action {#action}

## Definition

There are several kinds of actions:

* state transition actions;
* internal actions;
* entry actions;
* exit actions.

### State transition action

A state transition action, as its name suggests, is associated to a state transition. Such an action is executed whenever the associated state transition occurs.

In the following diagram, `action` is executed whenever the state machine transitions from `state0` to `state1` (because `event` occurs):

@startuml{state_transition_action.png} "A state transition action"
[*] --> state0
hide empty description
state0 -> state1 : event / action
@enduml

### Internal action

An internal action is associated to a state and an event. Such an action is executed whenever the associated event occurs while the associated state is active.

In the following diagram, `action` is executed whenever `event` occurs while `state0` is active.

@startuml{internal_action.png} "An internal action"
hide empty description
[*] --> state0
state0 : event / action
@enduml

### Entry action

An entry action is associated to a state. Such an action is executed whenever the state machine enters the associated state.

In the following diagram, `action` is executed whenever the state machine enters `state2`, whatever the source state (here `state0` or `state1`):

@startuml{entry_action.png} "An entry action"
hide empty description
[*] --> state0
state0 -> state1
state0 --> state2
state1 --> state2
state2 : entry / action
@enduml

### Exit action

An exit action is associated to a state. Such an action is executed whenever the state machine exits the associated state.

In the following diagram, `action` is executed whenever the state machine exits `state0`, whatever the target state (here `state1` or `state2`):

@startuml{exit_action.png} "An exit action"
hide empty description
[*] --> state0
state0 : exit / action
state0 --> state1
state0 --> state2
@enduml

## When to use which kind of action

Internal actions is the only kind of action that can be executed without a state transition. A set of internal actions expresses "when this state is active, here are the actions that must be executed for these types of event".

When you want to execute an action during a state transition however, you have several choices.

* Entry/exit actions being associated to a specific state, they are well suited for:
    * the initialization/deinitialization of the state (e.g. allocation/deallocation of resources, start/stop of timer to implement a timeout, etc.);
    * calling functions that are semantically associated with the state name (e.g. `start_motor()`/`stop_motor()` for a state named `running_motor`).
* State transition actions are well suited for executing functions that have more to do with the specific transition it's associated to than with the source or target state.

## How to define actions within Maki

There are two ways to define and associate an action:

* within the transition table (possible for state transition actions and internal actions);
* within the associated state (possible for internal actions, entry actions and exit actions).

### Within the transition table

The action is the fourth (optional) parameter of `maki::transition_table::operator()()`. In this context, Maki expects a callable that can be invoked in one of the following ways (from highest to lowest priority):

~~~{.cpp}
std::invoke(action, ctx, mach, event);
std::invoke(action, ctx, event);
std::invoke(action, ctx);
std::invoke(action);
~~~

What determines the kind of the action (between a state transition action and an internal action) is the target state (the third parameter) given to `maki::transition_table::operator()()`:

* if the target state is `maki::null`, the action is an internal action;
* if the target state is a valid state, the action is a state transition action.

Note: A transition without target state is called an internal transition. No state exit or entry happen in this case.

Here is an example of two actions, with their definition and their association with a transition:

@snippet concepts/action/src/main.cpp short-in-transition

### Within the associated state

To associate an action to a state, you have to add a callable to the state configurator through a call to either:

* `maki::state_conf::entry_action_v()` (and its variants) for an entry action;
* `maki::state_conf::internal_action_v()` (and its variants) for an internal action;
* `maki::state_conf::exit_action_v()` (and its variants) for an exit action.

Here is an example of a state that defines all three kinds of actions (entry, internal and exit):

@snippet concepts/action/src/main.cpp short-in-state

As you can see, for each action you have to specify:

* the signature of the action, specified by the suffix of the function name, such as:
    * `_e` for an action that only takes a reference to the event that triggers the action;
    * `_ce` for an action that takes a reference to the context, as well as a reference to the event;
    * `_v` for an action that doesn't take any argument;
    * and so on (see `maki::state_conf` for all suffixes)...;
* the event type for which the action is invoked, specified by the given template argument.

Order matters! Whenever the state machine must execute an action, it iterates over the provided action list until it finds a match (i.e. an action of the adequate kind, specifying the adequate event type).

### Example

Here is an example program for all the actions we've defined in this chapter:

@snippet concepts/action/src/main.cpp all

The output of this program is the following:

@include concepts/action/expected-output.txt
