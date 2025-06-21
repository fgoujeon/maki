# Context {#context}

As you already know, a state machine is associated to a context type whose instance can be accessed by all the actions and guards. But there's a little bit more than that.

## State Machine Context

Let's review the basics. The context of a state machine is an object of any type that can be accessed by all its parts (such as actions and guards). A `struct` such as this one is an adequate context type:

@snippet extra/context/machine/src/main.cpp context

The type of the context is an option of `maki::machine_conf`:

@snippet extra/context/machine/src/main.cpp machine-conf

You don't instantiate the context yourself; `maki::machine` does. This is why you must specify how Maki must construct it. Setting the context type with `maki::machine_conf::context_a()` like we just did indicates to Maki that we want it to construct the context by forwarding the arguments given to the constructor of `maki::machine`, and only these arguments. Indeed, the `_a` suffix stands for "arguments".

Then, when we instantiate the state machine like below, we indirectly construct the context by setting all its member variables:

@snippet extra/context/machine/src/main.cpp machine-instance

After setting all this up, we can finally access the context from our actions:

@snippet extra/context/machine/src/main.cpp state

Here is the program containing all the code snippets above in the right order:

@snippet extra/context/machine/src/main.cpp all

The output of this program is the following:

@include extra/context/machine/expected-output.txt

## State Context

Any state, however deeply nested in the [composite state](@ref composite-state) hierarchy, can have a context of its own as well.

By default, a state is associated with the context of its parent (i.e. the state machine or, in the case of a substate, its direct parent composite state). We can associate it with another one by calling `maki::state_mold::context_c()`, for example. The `_c` suffix refers to the machine context; we want Maki to construct the context of the state by passing it the context of its parent.

@snippet extra/context/state/src/main.cpp context

In case you're wondering: In the example above, we've been able to set the entry action like we did because in this case, `maki::state_mold::entry_action_c()` asks Maki to execute the action by calling `std::invoke(&my_state_context::on_entry, instance_of_my_state_context)`, which is equivalent to `instance_of_my_state_context.on_entry()`.

> [!important]
> State contexts have the same lifetime as the instance of `maki::machine`. They're *not* created at state entry and destructed at state exit!
