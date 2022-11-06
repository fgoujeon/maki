[![Build status](https://ci.appveyor.com/api/projects/status/1jvbol4cwrfivd3y/branch/master?svg=true)](https://ci.appveyor.com/project/fgoujeon/awesm/branch/master)

---

# AweSM
AweSM is a C++17 finite-state machine library.

**This library is still in early development stage: it is functional and tested, but its API is subject to change.**

## Features
AweSM implements the following key features:

* **transition tables**, featuring:
  * **actions**;
  * **guards**;
  * **internal transitions**, aka transitions to `null` state;
  * **completion transitions**, aka anonymous transitions, aka transitions through `null` event;
  * source state **pattern matching** with `any`, `any_of`, `any_but`, `any_if` and `any_if_not`;
* **states as classes**, featuring:
  * **entry/exit actions**, aka `on_entry()` and `on_exit()` member functions;
  * **internal transition actions**, aka `on_event()` member function;
* **run-to-completion**, the guarantee that the processing of an event won't be interrupted, even if we ask to handle other events in the process;
* **orthogonal regions**.

Besides its features, AweSM:

* **has excellent performance**, both at build time and runtime (see [benchmark](https://github.com/fgoujeon/fsm-benchmark));
* **doesn't depend on any library** other than the C++ standard library;
* **doesn't rely on exceptions**, while still allowing you to be exception-safe;
* **doesn't rely on RTTI**;
* is licensed under the terms of the very permissive **Boost Software License**, allowing you to use the library in any kind of free or proprietary software or firmware.

## Example
The following example is firmware for an RGB lamp. This lamp has a single button and an LED that can emit white, red, green or blue.

The expected behavior is:

* when the lamp is off, pushing the button turns it on in white color;
* then, briefly pushing the button changes the color of the LED, following this order: white -> red -> green -> blue -> white -> etc.;
* finally, whenever the user presses the button more than one second, the lamp turns off.

This behavior can be expressed with the following transition table:
```c++
using sm_transition_table = awesm::transition_table
<
    //  source_state,   event,       target_state,   action,            guard
    row<off,            button_push, emitting_white, turn_light_white>,
    row<emitting_white, button_push, emitting_red,   turn_light_red,    is_short_push>,
    row<emitting_red,   button_push, emitting_green, turn_light_green,  is_short_push>,
    row<emitting_green, button_push, emitting_blue,  turn_light_blue,   is_short_push>,
    row<emitting_blue,  button_push, emitting_white, turn_light_white,  is_short_push>,
    row<any_but<off>,   button_push, off,            turn_light_off,    is_long_push>
>;
```

Here is the full program:
```c++
#include <awesm.hpp>
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
            //Low-level stuff...
        }

    private:
        //Low-level stuff...
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
            //Low-level stuff...
        }

    private:
        color color_ = color::off;
        //Low-level stuff...
};

/*
An instance of this class is shared by all the states, actions and guards of the
state machine.
*/
struct context
{
    rgb_led led;
};

/*
States are classes.
*/
namespace states
{
    /*
    A state class must be constructible with one of the following expressions:
        auto state = state_type{machine, context};
        auto state = state_type{context};
        auto state = state_type{};
    */
    struct off
    {
        /*
        A state class must define a conf subtype.
        */
        using conf = awesm::state_conf
        <
            /*
            With this option, we require the state machine to call an on_entry()
            function whenever it enters our state.
            One of these expressions must be valid:
                state.on_entry(event);
                state.on_entry();
            Where `event` is the event that caused the state transition.
            */
            awesm::state_options::on_entry_any,

            /*
            Here, we require the state machine to call an on_event() function
            whenever it processed an event while our state is active. We also
            indicate that we want it to do so only when the type of the said
            event is button::push_event.
            This expression must be valid:
                state.on_event(event);
            */
            awesm::state_options::on_event_any_of<button::push_event>,

            /*
            Finally, we want the state machine to call on_exit() whenever it
            exits our state.
            One of these expressions must be valid:
                state.on_exit(event);
                state.on_exit();
            Where `event` is the event that caused the state transition.
            */
            awesm::state_options::on_exit_any
        >;

        void on_entry(const button::push_event& event)
        {
            std::cout << "Turned off after a ";
            std::cout << event.duration_ms << " millisecond push\n";
        }

        void on_entry(const awesm::null& /*event*/)
        {
            std::cout << "Started state machine\n";
        }

        void on_event(const button::push_event& event)
        {
            std::cout << "Received a ";
            std::cout << event.duration_ms;
            std::cout << " millisecond push in off state\n";
        }

        void on_exit()
        {
            std::cout << "Turned on\n";
        }

        context& ctx;
    };

    /*
    These are minimal valid state classes.
    */
    struct emitting_white { using conf = awesm::state_conf<>; };
    struct emitting_red { using conf = awesm::state_conf<>; };
    struct emitting_green { using conf = awesm::state_conf<>; };
    struct emitting_blue { using conf = awesm::state_conf<>; };
}

/*
Actions are classes.
*/
namespace actions
{
    /*
    An action class is required to implement the execute() function described
    below.
    Also, just like state classes, action classes must be constructible with one
    of the following expressions:
        auto action = action_type{machine, context};
        auto action = action_type{context};
        auto action = action_type{};
    */
    struct turn_light_off
    {
        /*
        Whenever a state machine executes an action, it calls the execute()
        function of that action.
        One of these expressions must be valid:
            action.execute(event);
            action.execute();
        */
        void execute()
        {
            ctx.led.set_color(rgb_led::color::off);
        }

        context& ctx;
    };

    //We can of course factorize with a template.
    template<auto Color>
    struct turn_light_tpl
    {
        void execute()
        {
            ctx.led.set_color(Color);
        }
        context& ctx;
    };
    using turn_light_white = turn_light_tpl<rgb_led::color::white>;
    using turn_light_red   = turn_light_tpl<rgb_led::color::red>;
    using turn_light_green = turn_light_tpl<rgb_led::color::green>;
    using turn_light_blue  = turn_light_tpl<rgb_led::color::blue>;
}

/*
Guards are classes.
*/
namespace guards
{
    /*
    A guard class is required to implement the check() function described below.
    Also, just like state classes, guard classes must be constructible with one
    of the following expressions:
        auto guard = guard_type{machine, context};
        auto guard = guard_type{context};
        auto guard = guard_type{};
    */
    struct is_long_push
    {
        /*
        Whenever a state machine checks a guard, it calls the check() function
        of that guard.
        One of these expressions must be valid:
            guard.check(event);
            guard.check();
        */
        bool check(const button::push_event& event)
        {
            return event.duration_ms > 1000;
        }
    };

    //We can use guard operators to compose guards.
    using is_short_push = awesm::not_<is_long_push>;
}

//Allow shorter names in transition table
using namespace states;
using namespace actions;
using namespace guards;
using button_push = button::push_event;
using awesm::row;
using awesm::any_but;

/*
This is the transition table. This is where we define the actions that must be
executed depending on the active state and the event we receive.
Basically, whenever awesm::sm::process_event() is called, AweSM iterates
over the rows of this table until it finds a match, i.e. when:
- 'source_state' is the currently active state (or is awesm::any);
- 'event' is the type of the processed event;
- and the 'guard' returns true (or is awesm::null).
When a match is found, AweSM:
- exits 'source_state';
- marks 'target_state' as the new active state;
- executes the 'action';
- enters 'target_state'.
The initial active state of the state machine is the first state encountered in
the transition table ('off', is our case).
*/
using sm_transition_table = awesm::transition_table
<
    //  source_state,   event,       target_state,   action,            guard
    row<off,            button_push, emitting_white, turn_light_white>,
    row<emitting_white, button_push, emitting_red,   turn_light_red,    is_short_push>,
    row<emitting_red,   button_push, emitting_green, turn_light_green,  is_short_push>,
    row<emitting_green, button_push, emitting_blue,  turn_light_blue,   is_short_push>,
    row<emitting_blue,  button_push, emitting_white, turn_light_white,  is_short_push>,
    row<any_but<off>,   button_push, off,            turn_light_off,    is_long_push>
>;

/*
We have to define this struct to define our state machine. Here we just specify
the transition table, but we can put many options in it.
*/
struct sm_def
{
    using conf = awesm::sm_conf<sm_transition_table>;
};

/*
We finally have our state machine.
Note that we can pass a configuration struct as second template argument to fine
tune the behavior of our state machine.
*/
using sm_t = awesm::sm<sm_def>;

int main()
{
    /*
    We're responsible for instantiating our context ourselves. Also, as the
    states, actions and guards only holds a reference to this context and not
    a copy, it's also our responsibility to keep it alive until the state
    machine is destructed.
    */
    auto ctx = context{};

    /*
    When we instantiate the state machine, we also instantiate every state,
    action and guard mentionned in the transition table. Note that they're
    instantiated once and for all: no construction or destruction happens during
    state transitions.
    */
    auto sm = sm_t{ctx};

    //The state machine must be started.
    sm.start();

#if TESTING
    auto simulate_push = [&](const int duration_ms)
    {
        sm.process_event(button::push_event{duration_ms});
    };

    auto check = [](const bool b)
    {
        if(!b)
        {
            std::cerr << "Test failed\n";
            exit(1);
        }
    };

    check(sm.is_active_state<states::off>());
    check(ctx.led.get_color() == rgb_led::color::off);

    simulate_push(200);
    check(sm.is_active_state<states::emitting_white>());
    check(ctx.led.get_color() == rgb_led::color::white);

    simulate_push(200);
    check(sm.is_active_state<states::emitting_red>());
    check(ctx.led.get_color() == rgb_led::color::red);

    simulate_push(200);
    check(sm.is_active_state<states::emitting_green>());
    check(ctx.led.get_color() == rgb_led::color::green);

    simulate_push(200);
    check(sm.is_active_state<states::emitting_blue>());
    check(ctx.led.get_color() == rgb_led::color::blue);

    simulate_push(200);
    check(sm.is_active_state<states::emitting_white>());
    check(ctx.led.get_color() == rgb_led::color::white);

    simulate_push(1500);
    check(sm.is_active_state<states::off>());
    check(ctx.led.get_color() == rgb_led::color::off);

    std::cout << "Test succeeded\n";

    return 0;
#else
    auto btn = button
    {
        [&](const auto& event)
        {
            sm.process_event(event);
        }
    };

    while(true){}
#endif
}
```

## Acknowledgements
AweSM is greatly inspired by Boost.MSM, and more precisely by its functor front-end. Actually, AweSM was born because Boost.MSM was too slow to build large state machines (which is expected for a library that has been written in a time when variadic templates weren't supported by the language).
