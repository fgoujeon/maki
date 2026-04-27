# Event Deferral {#event-deferral}

## Definition

A state can define a set of events it defers. As long as any active state defers an event, the state machine postpones the processing of the occurrences of this event. Once it's no longer the case, the state machine processes the deferred event occurrences.

### Example

In the state diagram below, the `initializing` state defers the `play_button_press` event.

@startuml{event_deferral_example.png} "Example of event deferral"
hide empty description
[*] --> stopped
stopped --> initializing : start_button_press
initializing --> paused : end_of_initialization
paused --> playing : play_button_press
stopped <-- paused : stop_button_press
paused <-- playing : pause_button_press
stopped <-- playing : stop_button_press

initializing : play_button_press / defer
@enduml

Here is what happens if the `play_button_press` event is detected while the `initializing` state is active:

1. instead of processing the event, the state machine stores it in a waiting queue;
2. eventually, the `end_of_initialization` is emitted, which makes the state machine exit the `initializing` state for the `paused` state;
3. since the `paused` state doesn't defer the `play_button_press` event, the state machine removes the occurrence of this event from its waiting queue and processes it;
4. the state machine finally exits the `paused` state for the `playing` state, as expected.

## When to Use Event Deferral

Use event deferral if your system is sometimes unable to process an event (e.g. a user input, an error, etc.), but you still want to process it as soon as possible.

The example above is a typical one: the system can't process user requests because it needs a couple of seconds to initialize, but it still does it as soon as it's ready.

## How to Defer an Event with Maki

*Available since Maki 1.2.0*

Call `maki::state_mold::defer()` to add an event type to the set of deferred event types of a state:

@snippet concepts/event-deferral/src/main.cpp state-mold
