//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

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
    constexpr auto off = maki::state_builder{}
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
    constexpr auto emitting_white = maki::state_builder{}
        .context_v<emitting_white_data>()
        .entry_action_c([](emitting_white_data& data)
        {
            ++data.counter;
        })
    ;

    /*
    These are minimal valid state classes.
    */
    constexpr auto emitting_red = maki::state_builder{};
    constexpr auto emitting_green = maki::state_builder{};
    constexpr auto emitting_blue = maki::state_builder{};
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

The initial state of the state machine is the target of the transition from
`maki::init` (`off`, is our case).
*/
constexpr auto transition_table = maki::transition_table{}
    //source,        target,         event,       action,           guard
    (maki::init,     off)
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
