# State {#state}

## Definition

A state represents what a system (or part of a system) could be doing at any given instant.

The state that represents what the system is *currently* doing is called the **active state**.

## How to define states with Maki

You don't directly create state objects when using Maki. What you do is create *state builder* objects and give Maki references to these objects. Eventually, when you instantiate a state machine, Maki creates instances of `maki::state` with these builders.

A state builder is a `constexpr` variable of type `maki::state_builder` (which is actually a class template with plenty of template parameters you don't have to worry about):

@snippet concepts/state/src/main.cpp state-builder-default

You can set options through method chaining. [Entry and exit actions](@ref action) are examples of such options:

@snippet concepts/state/src/main.cpp state-builder-with-options
