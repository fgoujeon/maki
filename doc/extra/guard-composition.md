# Guard Composition {#guard-composition}

Sometimes it can be useful to call guards from another guard. For example, these two guards:

@snippet extra/guard-composition/src/main.cpp guards

... can be composed like so:

@snippet extra/guard-composition/src/main.cpp guard-composition

But this is a bit verbose and inconvenient.

Fortunately, Maki allows you to compose guards using boolean operators (`!`, `&&`, `||` and `!=`). For example:

@snippet extra/guard-composition/src/main.cpp better-guard-composition

You can also inline such expressions in transition tables:

@snippet extra/guard-composition/src/main.cpp better-guard-composition-in-tt
