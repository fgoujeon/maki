# Event {#event}

## Definition

TODO

## When to use an event

TODO

## How to use events within AweSM

Within AweSM, an event can be an object of any type. It can carry data that your state machine (more precisely your [actions](@ref action) and [guards](@ref guard), which we'll see later in this manual) will be able to read.

AweSM only requires your events to be copyable (unless you disable [run-to-completion](@ref run-to-completion), but we'll see that later as well).

Very often, you will define event types in modules that don't even depend on AweSM whatsoever.

### Example

Let's see how an AweSM-based program typically handles events.

We have a device that is made of:
* a motor that takes a little time to start and stop;
* a user interface with two buttons to require the motor to start or stop.

We then implement a monitor for these buttons (the `user_interface` class) as well as a driver for the motor (the `motor` class). Both these classes send several types of events (asynchronously). To keep our code terse, we wrap these events into `std::variant`s.

Here is `user_interface.hpp`:

@snippet events/src/user_interface.hpp all

Here is `motor.hpp`:

@snippet events/src/motor.hpp all

We need a state machine to make sure:
* we don't forward multiple start requests while the motor is starting;
* we don't forward multiple stop requests while the motor is stopping.

Here is the state diagram:

@startuml{motor_sm.png} "The state machine of a motor"
hide empty description
[*] --> idle
idle --> starting : user_interface::start_request / start_motor
starting --> running : motor::start_event
running --> stopping : user_interface::stop_request / stop_motor
stopping --> idle : motor::stop_event
@enduml

And here is `main.cpp`, which implements the state machine:

@snippet events/src/main.cpp all

Don't worry too much about the details for now. Just notice that:
* all the event types are defined into `user_interface` and `motor`, which don't depend on AweSM;
* all the events sent by `user_interface` and `motor` are forwarded to our `sm_t` instance through a call to [sm_t::process_event()](@ref awesm::sm::process_event).
