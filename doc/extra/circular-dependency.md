# Circular Dependency Resolution {#circular-dependency}

The various objects of a program that uses Maki depend on each other like so:

@startuml{object_dependency.png} "Dependency Between Maki Objects"

object machine
object machine_context
object state_mold
object state_context
object action
object guard

machine ..> machine_context
machine ..> state_context
machine ..> state_mold
machine ..> action
machine ..> guard

state_mold ..> state_context

@enduml

If you ever want to call `maki::machine::process_event()` from, say, an action, you'll have a circular dependency.

There are two ways you can solve this issue.

## Using a Template

Just use a template or a generic lambda:

@snippet extra/circular-dependency/template/src/main.cpp resolution

Pros:

* It's straightforward.
* It's zero-cost.

Cons:

* It forces you to write code into header files.

## Using a `maki::machine_ref`

`maki::machine_ref` is a type-erasing wrapper for a reference to a mutable `maki::machine`.

> [!note]
> In C++, type erasure is the process of storing an object into a container whose type doesn't expose the exact type of the object.

Since `maki::machine_ref` doesn't expose the type of the machine, it breaks the circular dependency.

@snippet extra/circular-dependency/machine_ref/src/main.cpp resolution

Pros:

* It allows you to write function bodies into implementation files.

Cons:

* You have to manually list the event types you need to process.
* It induces an extra cost due to the type erasure mechanism. It involves function pointers, which prevent the compiler from inlining.
