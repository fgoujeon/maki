# Catch Action {#catch-action}

## Definition

A **catch action** is an action that is invoked whenever an exception is caught by the state machine.

## Why You Need Catch Actions

Maki doesn't throw exceptions itself. There are however some cases where an exception may be thrown during a call to a member function of `maki::machine`:
- an action, a guard, or a hook throws an exception;
- an event is about to be copied into the @ref run-to-completion "run-to-completion" queue, but the copy constructor throws an exception;
- an event is about to be copied into the @ref run-to-completion "run-to-completion" queue, but memory allocation fails and throws an exception.

Maki provides basic exception guarantee; if the code it calls throws an exception, the state machine stays in a valid state.

However, by default, Maki doesn't catch these exceptions. If you want it to, you must set up a **catch action**, which inserts a `try`/`catch(...)` block at the topmost level of every non-`const` member function of `maki::machine`.

## How to Use Catch Actions

To set up a catch action, you must call `maki::machine_conf::catch_mx()`, like so:

@snippet doc/extra/exception-safety/src/main.cpp catch

The given callable is invoked from the inserted `catch(...)` blocks.

Processing some kind of error event, like in the above example, is a common way to recover from an exception. This can be coupled with an error state to which the topmost region transitions whenever this error event occurs, as described in @ref undefined-state.
