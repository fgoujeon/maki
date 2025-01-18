# State Sets {#state-set}

## Definition

A state set expresses a constraint that defines whether it contains any given state.

> [!note]
> Here, the word "set" is used in its mathematical sense, not in its `std::set` sense.

A state set can be "any state", "no state at all", or everything in between.

Maki exposes the `maki::state_set` class template to represent state sets. Internally, it's nothing more than a predicate, i.e. a function taking a `maki::state` and returning a `bool`.

To instantiate a `maki::state_set`, you have to compose `maki::state_conf` objects with boolean operators, such as:

@snippet extra/state-set/src/main.cpp state-set

## When to Use State Sets

You can use state sets as source states in transition tables to factorize transitions. In the example below, all the states, except `powering_off` itself, transition to `powering_off` whenever the state machine processes a `power_button_press` event:

@snippet extra/state-set/src/main.cpp transition-table
