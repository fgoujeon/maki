# Run-to-Completion {#run-to-completion}

## Definition

Run-to-completion is the guarantee that the processing of an event can't be interrupted by the processing of another event.

The simplest way to explain what run-to-completion is is to show what happens when it isn't guaranteed. Say you have a state machine of this form:

@startuml{rtc_diagram.png}
[*] --> A
hide empty description
A --> B : E0
B --> C : E1

A : exit / say_hello()

B : entry / emit_event_1_and_say_world()
B : exit / say_goodbye()
@enduml

Its actions are the following:

* `say_hello()`, which outputs `"Hello"`;
* `emit_event_1_and_say_world()`, which emits the event `E1` and outputs `", World!"` (in this order);
* `say_goodbye()`, which outputs `"\nGoodbye."`.

Say the active state is `A` and some external component emits the event `E0`. This makes the state machine transition from `A` to `B`, then from `B` to `C` since the entry action of `B` emits the event `E1`.

With run-to-completion, you'd have got this output:

@include concepts/run-to-completion/ok/expected-output.txt

Without run-to-completion, you get this one:

@include concepts/run-to-completion/nok/expected-output.txt

Frightening.

What is happening is that the processing of `E0` and the processing of `E1` are intertwined; the state machine:

1. performs some steps of the processing of `E0`;
2. processes `E1` from beginning to end;
3. performs the remaining steps of the processing of `E0`.

With run-to-completion, both events would have been fully processed sequentially.

## When to use run-to-completion

Always use run-to-completion, unless you can't afford its performance penalty.

This feature indeed comes at a cost; to guarantee run-to-completion, a state machine library must use a queue of some sort to store the events its client asks to process while it's busy. This might involve memory allocation.

Obviously, if you dare disabling it, be **absolutely sure** none of your actions asks to process an event, neither directly nor indirectly. But be assured that the bigger your program and your development team, the harder it is to enforce.

## How to enable run-to-completion within Maki

Being a safety feature, run-to-completion is enabled by default. You can disable it like so:

@snippet concepts/run-to-completion/ok/src/main.cpp machine-conf
