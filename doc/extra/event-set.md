# Event Type Sets {#event-set}

## Definition

An event type set expresses a constraint that defines whether it contains any given event type.

> [!note]
> Here, the word "set" is used in its mathematical sense, not in its `std::set` sense.

An event type set can be "any event type", "no event type at all", or everything in between.

Maki exposes the `maki::event_set` class template to represent event type sets. Internally, it's nothing more than a predicate, i.e. a function taking a `maki::event_t` and returning a `bool`.

The most common way of instantiating a `maki::event_set` is by composing `maki::event_t` objects with boolean operators, such as:

@snippet extra/event-set/in-transition-table/src/main.cpp event-set

## When to Use Event Type Sets

### In Transition Tables

You can use event type sets in transition tables to factorize transitions:

@snippet extra/event-set/in-transition-table/src/main.cpp transition-table

### As Filters for Actions and Hooks

You can use an event type set to define all the event types that can trigger an action:

@snippet extra/event-set/as-action-filter/src/main.cpp event-set
