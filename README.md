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
    //         start state,    event,      target state,   action,                      guard
    fgfsm::row<off,            push_event, emitting_white, turn_light_white>,
    fgfsm::row<emitting_white, push_event, emitting_red,   fgfsm::fn<turn_light_red>,   is_short_push>,
    fgfsm::row<emitting_red,   push_event, emitting_green, fgfsm::fn<turn_light_green>, is_short_push>,
    fgfsm::row<emitting_green, push_event, emitting_blue,  fgfsm::fn<turn_light_blue>,  is_short_push>,
    fgfsm::row<emitting_blue,  push_event, emitting_white, turn_light_white,            is_short_push>,
    fgfsm::row<fgfsm::any,     push_event, off,            fgfsm::fn<turn_light_off>,   is_long_push>
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
        See guards::is_long_push later in the code to see how the event can be
        accessed.
        */
        void on_entry(const fgfsm::any_cref& /*event*/)
        {
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
    An action class is required to:
    - be default-constructible;
    - implement the operator()() function described below.
    */
    struct turn_light_white
    {
        void operator()(context& ctx, const fgfsm::any_cref& /*event*/)
        {
            ctx.led.set_color(rgb_led::color::white);
        }
    };

    /*
    We can also define an action with a function, but it must be wrapped into
    an fgfsm::fn to turn it into a class.
    */
    void turn_light_red(context& ctx, const fgfsm::any_cref& /*event*/)
    {
        ctx.led.set_color(rgb_led::color::red);
    }

    void turn_light_green(context& ctx, const fgfsm::any_cref& /*event*/)
    {
        ctx.led.set_color(rgb_led::color::green);
    }

    void turn_light_blue(context& ctx, const fgfsm::any_cref& /*event*/)
    {
        ctx.led.set_color(rgb_led::color::blue);
    }

    void turn_light_off(context& ctx, const fgfsm::any_cref& /*event*/)
    {
        ctx.led.set_color(rgb_led::color::off);
    }
}

/*
Guards are classes.
*/
namespace guards
{
    /*
    A guard class is required to:
    - be default-constructible;
    - implement the operator()() function described below.
    */
    struct is_long_push
    {
        bool operator()(context& ctx, const fgfsm::any_cref& event)
        {
            auto long_push = false;

            /*
            An fgfsm::any_cref object is usually accessed through the
            fgfsm::visit() function, which takes the fgfsm::any_cref object,
            followed by any number of unary function objects.
            */
            fgfsm::visit
            (
                event,

                /*
                This function object is called if the actual type of the event
                is button::push_event.
                */
                [&](const button::push_event& event)
                {
                    if(event.duration_ms > 1000)
                        long_push = true;
                },

                /*
                This function object is called if the actual type of the event
                is int... which never happens in our example. This is just here
                as an illustration.
                */
                [&](const int /*event*/)
                {
                    //Won't happen.
                }
            );

            return long_push;
        }
    };

    using is_short_push = fgfsm::not_<is_long_push>;
}

//Allow shorter names in transition table
using namespace states;
using namespace actions;
using namespace guards;
using push_event = button::push_event;

using transition_table = fgfsm::transition_table
<
    //         start state,    event,      target state,   action,                      guard
    fgfsm::row<off,            push_event, emitting_white, turn_light_white>,
    fgfsm::row<emitting_white, push_event, emitting_red,   fgfsm::fn<turn_light_red>,   is_short_push>,
    fgfsm::row<emitting_red,   push_event, emitting_green, fgfsm::fn<turn_light_green>, is_short_push>,
    fgfsm::row<emitting_green, push_event, emitting_blue,  fgfsm::fn<turn_light_blue>,  is_short_push>,
    fgfsm::row<emitting_blue,  push_event, emitting_white, turn_light_white,            is_short_push>,
    fgfsm::row<fgfsm::any,     push_event, off,            fgfsm::fn<turn_light_off>,   is_long_push>
>;

struct fsm_configuration: fgfsm::fsm_configuration
{
    using context = ::context;
    using transition_table = ::transition_table;
};

using fsm = fgfsm::fsm<fsm_configuration>;

int main()
{
    auto ctx = context{};
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
