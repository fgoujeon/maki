# Exception Safety {#exception-safety}

Maki doesn't throw exceptions itself. There are however some cases where an exception may be thrown during a call to a member function of `maki::machine`:
- an action, a guard, or a hook throws an exception;
- an event is about to be copied into the @ref run-to-completion "run-to-completion" queue, but the copy constructor throws an exception;
- an event is about to be copied into the @ref run-to-completion "run-to-completion" queue, but memory allocation fails and throws an exception.

Maki provides basic exception guarantee; if the code it calls throws an exception, the state machine stays in a valid state.

By default, Maki doesn't catch anything. You can call `maki::machine_conf::catch_mx()` to set up a `try`/`catch(...)` block at the topmost level of every non-`const` member function of `maki::machine`. For example:

@snippet doc/extra/exception-safety/src/main.cpp catch

Processing some kind of error event, like in the above example, is a common way to recover from an exception.

This can be coupled with an error state to which the topmost region transitions whenever this error event occurs, like depicted in the diagram below:

@startuml{exception_recovery.png} "A typical setup for exception recovery"
state running {
    state whatever as "..."
    state whatever_2 as "..."
    state whatever_3 as "..."

    [*] --> whatever
    whatever -> whatever_2
    whatever_2 -> whatever_3
    whatever_3 -> whatever
}

[*] --> initializing
initializing --> running
running -> failing : error
running <- failing : error_acknowledgement

hide empty description
@enduml
