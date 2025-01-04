# Filter {#filter}

## Definition

A filter is a constraint used to select a valid subset of values. In Maki's API, filters can be applied on states and event types.

For example, to express any state that is neither `idle` nor `failing`, you'd write something like this:

```cpp
constexpr auto my_state_filter = maki::any_but(idle, failing);
```

## When to Use Filters

The places of the API where you can use filters are documented in the API reference. Here are some examples anyway.

### In Transition Tables

The main part of the API where filters are useful is the transition table. A state filter can be used in lieu of a source state, and an event filter can be used in lieu of an event.

For example, we'd like to implement the following state machine:

@startuml{filter_state_diagram.png} "State Diagram"
[*] --> initializing
hide empty description
initializing --> running : end of init
running -> sleeping : sleep button press
running <- sleeping : any button press (except off)
initializing --> powering_off : power button press
running --> powering_off : power button press
sleeping --> powering_off : power button press
@enduml

Implementing this state machine presents two difficulties:

* How do we factorize the many transitions to `powering_off`?
* How do we express the event "any button press (except off)" without enumerating all the event types that are not `power_button_press`?

The answer to both questions is: use filters. Respectively a source state filter and an event filter:

@snippet extra/filter/transition-table/src/main.cpp transition-table

### For State Actions

You're writing firmware for some device with a keyboard, and you're writing the configuration of a state that must beep whenever a digit button is pressed.

For some reason, the button press events are the following:

@snippet extra/filter/state-action/src/main.cpp events

Obviously, you're not going to write 10 internal actions; you use a filter instead:

@snippet extra/filter/state-action/src/main.cpp state-conf

## Available Filters

The following filters are available:

* `maki::any`;
* `maki::any_if()`;
* `maki::any_if_not()`;
* `maki::any_of()`;
* `maki::any_but()`;
* `maki::none`.

For convenience, Maki also defines these aliases that are adapted to types:

* `maki::any_type_of`, equivalent to `maki::any_of(maki::type<Types>...)`;
* `maki::any_type_but`, equivalent to `maki::any_but(maki::type<Types>...)`.

Also, note that a single value is a valid filter.
