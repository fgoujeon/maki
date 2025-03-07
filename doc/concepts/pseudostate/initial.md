# Initial {#initial}

## Definition

The initial pseudostate represents the starting point of a region; it indicates the state that is entered by default whenever a state machine starts or whenever a composite state is entered.

It is represented by a full circle. In the example below, it indicates that the `idle` state is active by default.

@startuml{initial_pseudostate.png}
[*] --> idle
hide empty description
@enduml

The initial pseudostate must obey these rules:

- in a given region, it cannot be the source of more than one transition;
- it cannot be the target of a transition;
- its outgoing transition cannot have an event nor a guard (but it can have an action).

## How to Use the Initial Pseudostate in Maki

In transition tables, the initial pseudostate is represented by the `maki::init` object. It must be the source state of the first transition of the table and cannot be used elsewhere.

> [!note]
> In theory, a composite state doesn't have to define initial pseudostates for its regions. A composite state can indeed be entered in other ways than the default one. But since Maki doesn't support these alternate ways yet, defining a transition from `maki::init` is mandatory.
