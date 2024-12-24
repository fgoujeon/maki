# Context {#context}

Until now, we've defined state machines whose actions and guards don't take arguments (except, at best, references to constant event objects). This only allows them to interact with global variables (such as `std::cout`) and call free functions. This is a bit limiting.

This is where **contexts** can help.

## State Machine Context

The context of a state machine is an object of any type that can be accessed by all its entities (i.e. its states, actions and guards). A `struct` such as this one is an adequate context type:

@snippet doc-concepts-context/src/main.cpp context

The type of the context is an option of `maki::machine_conf`:

@snippet doc-concepts-context/src/main.cpp machine-conf

You don't instantiate the context yourself; `maki::machine` does. This is why you must specify how Maki must construct it. Setting the context type with `maki::machine_conf::context_a()` like we just did indicates to Maki that we want it to construct the context by forwarding the arguments given to the constructor of `maki::machine`, and only these arguments. Indeed, the `_a` suffix stands for "arguments".

Then, when we instantiate the state machine like below, we indirectly construct the context by setting all its member variables:

@snippet doc-concepts-context/src/main.cpp machine-instance

After setting all this up, we can finally access the context from our actions:

@snippet doc-concepts-context/src/main.cpp state

Here is the program containing all the code snippets above in the right order:

@snippet doc-concepts-context/src/main.cpp all

The output of this program is the following:

@include doc-concepts-context/expected-output.txt

## State Context

States can have contexts of their own as well.

By default, a state is associated with the machine context. We can associate it with another one by calling `maki::state_conf::context_c()`, for example. The `_c` suffix refers to the machine context; we want Maki to construct the state context by passing it the machine context.

@snippet doc-concepts-context-state/src/main.cpp context

In case you're wondering: In the example above, we've been able to set the entry action like we did because in this case, `maki::state_conf::entry_action_c()` asks Maki to execute the action by calling `std::invoke(&my_state_context::on_entry, instance_of_my_state_context)`, which is equivalent to `instance_of_my_state_context.on_entry()`.

> [!important]
> State contexts have the same lifetime as the instance of `maki::machine`. They're *not* created at state entry and destructed at state exit!
