# FGFSM
FGFSM is a C++ Finite State Machine library.

## Features
FGFSM implements the following key features:

* **transition tables**, featuring:
  * **actions**;
  * **guards**;
  * **internal transitions**, aka transitions to `none` state;
  * **anonymous transitions**, aka transitions through `none` event;
  * **transition factorizations**, aka transitions from `any` state;
* **state entry/exit actions** as state members (`on_entry()` and `on_exit()`);
* **internal transitions actions** as state member (`on_event()`);
* **sane build time**, thanks to:
  * the low build-time complexity (O(n), n being the number of rows in the transition table) of the template instantiation of `fgfsm::fsm`;
  * the fact that `fgfsm::fsm::process_event()` is not a template function;
  * the fact that the user code doesn't *have to* define templates.

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
    //         start state,    event,      target state,   action,            guard
    fgfsm::row<off,            push_event, emitting_white, turn_light_white>,
    fgfsm::row<emitting_white, push_event, emitting_red,   turn_light_red,    is_short_push>,
    fgfsm::row<emitting_red,   push_event, emitting_green, turn_light_green,  is_short_push>,
    fgfsm::row<emitting_green, push_event, emitting_blue,  turn_light_blue,   is_short_push>,
    fgfsm::row<emitting_blue,  push_event, emitting_white, turn_light_white,  is_short_push>,
    fgfsm::row<fgfsm::any,     push_event, off,            turn_light_off,    is_long_push>
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
    A state class is required to implement the on_entry(), on_event() and
    on_exit() functions described below.
    Also, it must be constructible with a reference to the context. Since FGFSM
    instantiates its states using aggregate initialization, an explicit
    constructor isn't necessary. Declaring a public member variable like below
    is enough.
    */
    struct off
    {
        /*
        This function is called whenever the FSM enters this state.
        The event that caused the state transition is given as argument. The
        event is wrapped into an fgfsm::any_cref object.
        */
        void on_entry(const fgfsm::any_cref& event)
        {
            /*
            fgfsm::any_cref is a std::any-like container that stores a reference
            to a const object. FGFSM provides visitation functions to access
            fgfsm::any_cref objects in a safe and concise way.
            fgfsm::visit() takes an fgfsm::any_cref and a series of unary
            function objects. The function object whose parameter type matches
            the type of the object wrapped into the fgfsm::any_cref gets called.
            */
            fgfsm::visit
            (
                event,
                [&](const button::push_event& event)
                {
                    std::cout << "Turned off after a ";
                    std::cout << event.duration_ms << " millisecond push\n";
                }
            );
        }

        /*
        This function is called whenever fgfsm::fsm::process_event() is called,
        provided this state is active.
        */
        void on_event(const fgfsm::any_cref& /*event*/)
        {
        }

        /*
        This function is called whenever the FSM exits this state.
        */
        void on_exit(const fgfsm::any_cref& /*event*/)
        {
        }

        context& ctx;
    };

    /*
    In this example, since we don't need entry/exit actions or internal
    transitions, we can declare our states with FGFSM_SIMPLE_STATE.
    */
    FGFSM_SIMPLE_STATE(emitting_white);
    FGFSM_SIMPLE_STATE(emitting_red);
    FGFSM_SIMPLE_STATE(emitting_green);
    FGFSM_SIMPLE_STATE(emitting_blue);
}

/*
Actions are classes.
*/
namespace actions
{
    /*
    An action class is required to implement the execute() function described
    below.
    Also, just like state classes, action classes must be constructible with a
    reference to the context.
    */
    struct turn_light_off
    {
        void execute(const fgfsm::any_cref& /*event*/)
        {
            ctx.led.set_color(rgb_led::color::off);
        }

        context& ctx;
    };

    //We can of course factorize with a template.
    template<auto Color>
    struct turn_light_tpl
    {
        void execute(const fgfsm::any_cref& /*event*/)
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
    Also, just like state classes, guard classes must be constructible with a
    reference to the context.
    */
    struct is_long_push
    {
        bool check(const fgfsm::any_cref& event)
        {
            /*
            fgfsm::any_cref objects are usually accessed through visitation
            functions like this one.
            fgfsm::visit_or_false() acts like fgfsm::visit() (seen earlier in
            the program) except that it returns the boolean returned by the
            matching function object, or false if no match is found.
            */
            return fgfsm::visit_or_false
            (
                event,
                [](const button::push_event& event)
                {
                    return event.duration_ms > 1000;
                }
            );
        }

        context& ctx;
    };

    /*
    Admittedly, the guard above is quite verbose.
    We can write guards much more concisely by using the fgfsm::guard_fn
    adapter, which internally calls fgfsm::visit_or_false().
    */
    bool is_short_push_impl(context& ctx, const button::push_event& event)
    {
        return event.duration_ms <= 1000;
    }
    using is_short_push = fgfsm::guard_fn<is_short_push_impl>;

    /*
    We could have written is_short_push like below.
    */
    using is_short_push_2 = fgfsm::not_<is_long_push>;
}

//Allow shorter names in transition table
using namespace states;
using namespace actions;
using namespace guards;
using push_event = button::push_event;

/*
This is the transition table. This is where we define the actions that must be
executed depending on the active state and the event we receive.
Basically, whenever fgfsm::fsm::process_event() is called, FGFSM iterates over
the rows of this table until it finds a match, i.e. when:
- 'start state' is the currently active state (or is fgfsm::any);
- 'event' is the type of the processed event;
- and the 'guard' returns true (or is fgfsm::none).
When a match is found, the 'action' is executed and 'target state' becomes the
new active state.
The initial active state of the FSM is the first state encountered in the
transition table (off, is our case).
*/
using transition_table = fgfsm::transition_table
<
    //         start state,    event,      target state,   action,            guard
    fgfsm::row<off,            push_event, emitting_white, turn_light_white>,
    fgfsm::row<emitting_white, push_event, emitting_red,   turn_light_red,    is_short_push>,
    fgfsm::row<emitting_red,   push_event, emitting_green, turn_light_green,  is_short_push>,
    fgfsm::row<emitting_green, push_event, emitting_blue,  turn_light_blue,   is_short_push>,
    fgfsm::row<emitting_blue,  push_event, emitting_white, turn_light_white,  is_short_push>,
    fgfsm::row<fgfsm::any,     push_event, off,            turn_light_off,    is_long_push>
>;

/*
We finally have our FSM.
Note that we can pass a configuration struct as second template argument to fine
tune the behavior of our FSM.
*/
using fsm = fgfsm::fsm<transition_table>;

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
FGFSM is greatly inspired by Boost.MSM, and more precisely by its function front-end.

Actually, FGFSM was born because Boost.MSM was too slow to build large FSMs (which is expected for a library that has been written in a time when variadic templates weren't supported by the language).
