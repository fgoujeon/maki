# Run-to-Completion {#run-to-completion}

## Definition

Run-to-completion is the guarantee that the processing of an event can't be interrupted by the processing of another event.

The simplest way to explain what run-to-completion is is to show what happens when it isn't guaranteed. Say you have a state machine of this form:

@startuml{rtc_diagram.png}
[*] --> A
A --> B : E0

A : exit / say_hello_and_emit_E1()

B : E1 / say_world()

hide empty description
@enduml

Its actions are the following:

* `say_hello_and_emit_E1()`, which outputs `"Hello,"` and emits the event `E1` (in this order);
* `say_world()`, which outputs `" World!"`.

Whenever the active state is `A` and the event `E0` occurs, the state machine transitions from `A` to `B` and processes the event `E1`.

With run-to-completion, you'd have gotten this output:

@include concepts/run-to-completion/ok/expected-output.txt

Without run-to-completion, you get this one:

@include concepts/run-to-completion/nok/expected-output.txt

Frightening.

What is happening is that the processing of `E0` and the processing of `E1` are intertwined; the state machine:

1. performs some steps of the processing of `E0` (namely, invoking the exit action of `A`);
2. processes `E1` from beginning to end;
3. performs the remaining steps of the processing of `E0` (namely, invoking the entry action of `B` and marking it as the active state).

Because of this, the state machine processes `E1` before it enters `B`, which results in `say_world()` not being called.

With run-to-completion, both events would have been fully processed in a sequential way.

## When to use run-to-completion

Always use run-to-completion, unless you can't afford its performance penalty.

This feature indeed comes at a cost; to guarantee run-to-completion, a state machine library must use a queue of some sort to store the events its client asks to process while it's busy. This might involve memory allocation.

Obviously, if you dare disabling it, be **absolutely sure** none of your actions asks to process an event, neither directly nor indirectly. But be assured that the bigger your program and your development team, the harder it is to enforce.

## How to enable run-to-completion within Maki

Being a safety feature, run-to-completion is enabled by default. You can disable it like so:

@snippet concepts/run-to-completion/ok/src/main.cpp machine-conf
