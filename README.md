[![Build status](https://ci.appveyor.com/api/projects/status/c82pwtf6q4bjeima/branch/master?svg=true)](https://ci.appveyor.com/project/fgoujeon/fgfsm/branch/master)

---

# FGFSM
FGFSM is a C++17 Finite State Machine library.

## Features
FGFSM implements the following key features:

* **transition tables**, featuring:
  * **actions**;
  * **guards**;
  * **internal transitions**, aka transitions to `none` state;
  * **completion transitions**, aka anonymous transitions, aka transitions through `none` event;
  * **Kleene start states**, aka transitions from `any` state;
* **states as classes**, featuring:
  * **entry/exit actions**, aka `on_entry()` and `on_exit()` member functions;
  * **internal transition actions**, aka `on_event()` member function;
* **run-to-completion**, the guarantee that the processing of an event won't be interrupted, even if we ask to handle other events in the process.

Besides its features, FGFSM:

* **doesn't depend on any library** other than the C++ standard library;
* **doesn't rely on exceptions**, while still allowing you to be exception-safe;
* **doesn't rely on RTTI**;
* is licensed under the terms of the **Boost Software License**, allowing you to use the library in any kind of free or proprietary software or firmware.

## Example
The following example is firmware for an RGB lamp. This lamp has a single button and an LED that can emit white, red, green or blue.

The expected behavior is:

* when the lamp is off, pushing the button turns it on in white color;
* then, briefly pushing the button changes the color of the LED, following this order: white -> red -> green -> blue -> white -> etc.;
* finally, whenever the user presses the button more than one second, the lamp turns off.

This behavior can be expressed with the following transition table:
```c++
using transition_table = fgfsm::transition_table
<
    //         start state,    event,       target state,   action,            guard
    fgfsm::row<off,            button_push, emitting_white, turn_light_white>,
    fgfsm::row<emitting_white, button_push, emitting_red,   turn_light_red,    is_short_push>,
    fgfsm::row<emitting_red,   button_push, emitting_green, turn_light_green,  is_short_push>,
    fgfsm::row<emitting_green, button_push, emitting_blue,  turn_light_blue,   is_short_push>,
    fgfsm::row<emitting_blue,  button_push, emitting_white, turn_light_white,  is_short_push>,
    fgfsm::row<fgfsm::any,     button_push, off,            turn_light_off,    is_long_push>
>;
```

Here is the full program:
```c++
#include <fgfsm.hpp>
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
FSM.
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
    A state class is required to implement the on_entry() and on_exit()
    functions described below.
    Also, it must be either constructible with a reference to the context or
    default-constructible. Since FGFSM instantiates its states using aggregate
    initialization, an explicit constructor isn't necessary. Declaring a public
    member variable like below is enough.
    */
    struct off
    {
        /*
        Whenever an FSM enters a state, it calls the on_entry() function of that
        state. It tries to do so using the following statements, in that order,
        until it finds a valid one:
            state.on_entry(event);
            state.on_entry();
        If no valid statement is found, a build error occurs.
        */
        void on_entry(const button::push_event& event)
        {
            std::cout << "Turned off after a ";
            std::cout << event.duration_ms << " millisecond push\n";
        }

        /*
        Optionally, state types can define a set of on_event() functions.
        Whenever the FSM processes an event, it calls the on_event() function of
        the active state by passing it the event (provided this function
        exists).
        The FSM does this call just before processing the event in the
        transition table.
        */
        void on_event(const button::push_event& event)
        {
            std::cout << "Received a ";
            std::cout << event.duration_ms;
            std::cout << " millisecond push in off state\n";
        }

        /*
        Whenever an FSM exits a state, it calls the on_exit() function of that
        state. It uses the same mechanism as the one used for on_entry().
        */
        void on_exit()
        {
        }

        context& ctx;
    };

    /*
    Empty state types are not required to implement on_entry() and on_exit().
    */
    struct emitting_white{};
    struct emitting_red{};
    struct emitting_green{};
    struct emitting_blue{};
}

/*
Actions are classes.
*/
namespace actions
{
    /*
    An action class is required to implement the execute() function described
    below.
    Also, just like state classes, action classes must be either constructible
    with a reference to the context or default-constructible.
    */
    struct turn_light_off
    {
        /*
        Whenever an FSM executes an action, it calls the execute() function of
        that action. It tries to do so using the following statements, in that
        order, until it finds a valid one:
            action.execute(event);
            action.execute();
        If no valid statement is found, a build error occurs.
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
    A guard class is required to implement the check() function described
    below.
    Also, just like state classes, guard classes must be either constructible
    with a reference to the context or default-constructible.
    */
    struct is_long_push
    {
        /*
        Whenever an FSM checks a guard, it calls the check() function of that
        guard. It tries to do so using the following statements, in that order,
        until it finds a valid one:
            guard.check(event);
            guard.check();
        If no valid statement is found, a build error occurs.
        */
        bool check(const button::push_event& event)
        {
            return event.duration_ms > 1000;
        }
    };

    //We can use guard operators to combine our guards.
    using is_short_push = fgfsm::not_<is_long_push>;
}

//Allow shorter names in transition table
using namespace states;
using namespace actions;
using namespace guards;
using button_push = button::push_event;

struct fsm_configuration: fgfsm::fsm_configuration
{
    /*
    This is the transition table. This is where we define the actions that must
    be executed depending on the active state and the event we receive.
    Basically, whenever fgfsm::fsm::process_event() is called, FGFSM iterates
    over the rows of this table until it finds a match, i.e. when:
    - 'start_state' is the currently active state (or is fgfsm::any);
    - 'event' is the type of the processed event;
    - and the 'guard' returns true (or is fgfsm::none).
    When a match is found, FGFSM:
    - exits 'start_state';
    - marks 'target_state' as the new active state;
    - executes the 'action';
    - enters 'target_state'.
    The initial active state of the FSM is the first state encountered in the
    transition table ('off', is our case).
    */
    using transition_table_t = fgfsm::transition_table
    <
        //         start_state,    event,       target_state,   action,            guard
        fgfsm::row<off,            button_push, emitting_white, turn_light_white>,
        fgfsm::row<emitting_white, button_push, emitting_red,   turn_light_red,    is_short_push>,
        fgfsm::row<emitting_red,   button_push, emitting_green, turn_light_green,  is_short_push>,
        fgfsm::row<emitting_green, button_push, emitting_blue,  turn_light_blue,   is_short_push>,
        fgfsm::row<emitting_blue,  button_push, emitting_white, turn_light_white,  is_short_push>,
        fgfsm::row<fgfsm::any,     button_push, off,            turn_light_off,    is_long_push>
    >;
};

/*
We finally have our FSM.
Note that we can pass a configuration struct as second template argument to fine
tune the behavior of our FSM.
*/
using fsm = fgfsm::fsm<fsm_configuration>;

int main()
{
    /*
    We're responsible for instantiating our context ourselves. Also, as the
    states, actions and guards only holds a reference to this context and not
    a copy, it's also our responsibility to keep it alive until the FSM is
    destructed.
    */
    auto ctx = context{};

    /*
    When we instantiate the FSM, we also instantiate every state, action and
    guard mentionned in the transition table. Note that they're instantiated
    once and for all: no construction or destruction happens during state
    transitions.
    */
    auto sm = fsm{ctx};

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

## Credits
FGFSM is greatly inspired by Boost.MSM, and more precisely by its function front-end. Actually, FGFSM was born because Boost.MSM was too slow to build large FSMs (which is expected for a library that has been written in a time when variadic templates weren't supported by the language).
