[![Windows Build status](https://ci.appveyor.com/api/projects/status/2vc1wsgwg7bo9y45/branch/main?svg=true)](https://ci.appveyor.com/project/fgoujeon/maki/branch/main)

[![Linux build status](https://github.com/fgoujeon/maki/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/fgoujeon/maki/actions/workflows/ci.yml)

---

# Maki
Maki is a C++17 finite-state machine library.

**This library is still in early development stage: it is functional and tested, but its API is subject to change.**

## Features
Maki implements the following key features:

* **transition tables**, featuring:
  * **actions**;
  * **guards**;
  * **internal transitions**;
  * **completion transitions**, aka anonymous transitions;
  * **event type sets**;
  * **state sets**;
* **states**, featuring:
  * **entry/exit actions**;
  * **internal transition actions**;
  * **associated data**;
* **run-to-completion**;
* **orthogonal regions**;
* **composite states**.

Besides its features, Maki:

* **has excellent performance**, both at build time and runtime (see [benchmark](https://github.com/fgoujeon/fsm-benchmark));
* **doesn't depend on any library** other than the C++ standard library;
* **doesn't rely on exceptions**, while still allowing you to be exception-safe;
* **doesn't rely on RTTI**;
* is licensed under the terms of the very permissive **Boost Software License**, allowing you to use the library in any kind of free or proprietary software or firmware.

What is *not* implemented (yet):

* elaborate ways to enter and exit a composite state (e.g. forks, history and exit points);
* event deferral;
* optional thread safety with mutexes.

## Documentation
You can access the full documentation [here](https://fgoujeon.github.io/maki/doc/v1).

## Example
The following example is firmware for an RGB lamp. This lamp has a single button and an LED that can emit white, red, green or blue.

The expected behavior is:

* when the lamp is off, pushing the button turns it on in white color;
* then, briefly pushing the button changes the color of the LED, following this order: white -> red -> green -> blue -> white -> etc.;
* finally, whenever the user presses the button more than one second, the lamp turns off.

This behavior can be expressed with the following transition table:
```c++
constexpr auto transition_table = maki::transition_table{}
    //source,        target,         event,       action,           guard
    (off,            emitting_white, button_push, turn_light_white)
    (emitting_white, emitting_red,   button_push, turn_light_red,   is_short_push)
    (emitting_red,   emitting_green, button_push, turn_light_green, is_short_push)
    (emitting_green, emitting_blue,  button_push, turn_light_blue,  is_short_push)
    (emitting_blue,  emitting_white, button_push, turn_light_white, is_short_push)
    (!off,           off,            button_push, turn_light_off,   is_long_push)
;
```

Here is the full program:
```c++
#include <maki.hpp>
#include <functional>
#include <iostream>

/*
This class monitors the button. It sends an event of type push_event whenever
the user pushes the button.
*/
class button
{
public:
    struct push_event
    {
        //Push duration, in milliseconds
        int duration_ms = 0;
    };

    using event_handler = std::function<void(const push_event&)>;

    button(const event_handler& /*eh*/)
    {
        //Implementation detail...
    }

private:
    //Implementation detail...
};

//This class drives the RGB LED.
class rgb_led
{
public:
    enum class color
    {
        off,
        white,
        red,
        green,
        blue
    };

    color get_color() const
    {
        return color_;
    }

    void set_color(const color c)
    {
        color_ = c;
        //Implementation detail...
    }

private:
    color color_ = color::off;
    //Implementation detail...
};

/*
An instance of this class is instantiated by the constructor of the state
machine. This instance can be accessed by every element (states, actions,
guards, ...) of the state machine.
*/
struct context
{
    rgb_led led;
};

/*
States are represented by constexpr objects.
*/
namespace states
{
    constexpr auto off = maki::state_conf{}
        /*
        Entry action invoked whenever the state machine enters the `off` state
        with a `button::push_event`.
        */
        .entry_action_e<button::push_event>([](const button::push_event& event)
        {
            std::cout << "Turned off after a ";
            std::cout << event.duration_ms << " millisecond push\n";
        })

        /*
        Entry action invoked whenever the state machine enters the `off` state
        with a state machine start event.
        */
        .entry_action_v<maki::events::start>([]
        {
            std::cout << "Started state machine\n";
        })

        /*
        Exit action invoked whenever the state machine exits the `off` state,
        whatever the type of the event that caused the state transition.
        */
        .exit_action_v([]
        {
            std::cout << "Turned on\n";
        })
    ;

    /*
    States can have their own private data.
    */
    struct emitting_white_data
    {
        int counter = 0;
    };
    constexpr auto emitting_white = maki::state_conf{}
        .context_v<emitting_white_data>()
        .entry_action_c([](emitting_white_data& data)
        {
            ++data.counter;
        })
    ;

    /*
    These are minimal valid state classes.
    */
    constexpr auto emitting_red = maki::state_conf{};
    constexpr auto emitting_green = maki::state_conf{};
    constexpr auto emitting_blue = maki::state_conf{};
}

/*
An action is a constexpr object holding a callable invoked whenever a specific
state transition occurs.
*/
namespace actions
{
    constexpr auto turn_light_off = maki::action_c([](context& ctx)
    {
        ctx.led.set_color(rgb_led::color::off);
    });

    //We can of course factorize with a template.
    template<auto Color>
    constexpr auto turn_light_tpl = maki::action_c([](context& ctx)
    {
        ctx.led.set_color(Color);
    });
    constexpr auto turn_light_white = turn_light_tpl<rgb_led::color::white>;
    constexpr auto turn_light_red   = turn_light_tpl<rgb_led::color::red>;
    constexpr auto turn_light_green = turn_light_tpl<rgb_led::color::green>;
    constexpr auto turn_light_blue  = turn_light_tpl<rgb_led::color::blue>;
}

/*
A guard is a constexpr object holding a callable invoked to check whether a
state transition must occur.
*/
namespace guards
{
    constexpr auto is_long_push = maki::guard_e([](const button::push_event& event)
    {
        return event.duration_ms > 1000;
    });

    //We can use boolean operators to compose guards.
    constexpr auto is_short_push = !is_long_push;
}

using namespace states;
using namespace actions;
using namespace guards;
inline constexpr auto button_push = maki::event<button::push_event>;

/*
This is the transition table. This is where we define the actions that the state
machine must execute depending on the active state and the event it receives.

Basically, whenever `maki::machine::process_event()` is called, Maki iterates
over the transitions of this table until it finds a match, i.e. when:
- `source` is the currently active state;
- `event` is the type of the processed event;
- and `guard` returns true.

When a match is found, Maki:
- exits `source`;
- marks `target` as the new active state;
- invokes `action`;
- enters `target`.

The initial active state of the state machine is the first state encountered in
the transition table (`off`, is our case).
*/
constexpr auto transition_table = maki::transition_table{}
    //source,        target,         event,       action,           guard
    (off,            emitting_white, button_push, turn_light_white)
    (emitting_white, emitting_red,   button_push, turn_light_red,   is_short_push)
    (emitting_red,   emitting_green, button_push, turn_light_green, is_short_push)
    (emitting_green, emitting_blue,  button_push, turn_light_blue,  is_short_push)
    (emitting_blue,  emitting_white, button_push, turn_light_white, is_short_push)
    (!off,           off,            button_push, turn_light_off,   is_long_push)
;

/*
We have to define this variable to configure our state machine.
Here, we just specify the transition table and the context type, but we can
configure many other aspects of the state machine.
*/
constexpr auto machine_conf = maki::machine_conf{}
    .transition_tables(transition_table)
    .context_a<context>()
;

/*
We finally have our configured state machine.
*/
using machine_t = maki::machine<machine_conf>;

int main()
{
    /*
    When we instantiate the state machine, we also instantiate:
    - a context;
    - the data types of the states mentioned in the transition table.

    Note that the state data types are instantiated once and for all: no
    construction or destruction happens during state transitions.
    */
    auto machine = machine_t{};
    auto& ctx = machine.context();

#if TESTING
    auto simulate_push = [&](const int duration_ms)
    {
        std::cout << "Button push (" << duration_ms << " ms)\n";
        machine.process_event(button::push_event{duration_ms});
    };

    auto check = [](const bool b)
    {
        if(!b)
        {
            std::cerr << "NOK\n";
            std::cerr << "Test failed\n";
            exit(1);
        }

        std::cout << "OK\n";
    };

    check(machine.is<states::off>());
    check(ctx.led.get_color() == rgb_led::color::off);

    simulate_push(200);
    check(machine.is<states::emitting_white>());
    check(ctx.led.get_color() == rgb_led::color::white);

    simulate_push(200);
    check(machine.is<states::emitting_red>());
    check(ctx.led.get_color() == rgb_led::color::red);

    simulate_push(200);
    check(machine.is<states::emitting_green>());
    check(ctx.led.get_color() == rgb_led::color::green);

    simulate_push(200);
    check(machine.is<states::emitting_blue>());
    check(ctx.led.get_color() == rgb_led::color::blue);

    simulate_push(200);
    check(machine.is<states::emitting_white>());
    check(ctx.led.get_color() == rgb_led::color::white);

    simulate_push(1500);
    check(machine.is<states::off>());
    check(ctx.led.get_color() == rgb_led::color::off);

    std::cout << "Test succeeded\n";

    return 0;
#else
    /*
    A real-life program would do something like this.
    */

    auto btn = button
    {
        [&](const auto& event)
        {
            machine.process_event(event);
        }
    };

    while(true){}
#endif
}
```

## Acknowledgements
Maki is greatly inspired by Boost.MSM. Actually, Maki was born because Boost.MSM was too slow to build large state machines (which is expected for a library that has been written in a time when variadic templates weren't supported by the language). Thank you Christophe Henry for your work.
