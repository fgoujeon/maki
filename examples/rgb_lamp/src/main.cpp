//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

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
        //--snip--
    }

private:
    //--snip--
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
        //--snip--
    }

private:
    color color_ = color::off;
    //--snip--
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
States are represented by classes.
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
            awesm::state_opts::on_entry<>,

            /*
            Here, we require the state machine to call an on_event() function
            whenever it processed an event while our state is active. We also
            indicate that we want it to do so only when the type of the said
            event is button::push_event.
            This expression must be valid:
                state.on_event(event);
            */
            awesm::state_opts::on_event<button::push_event>,

            /*
            Finally, we want the state machine to call on_exit() whenever it
            exits our state.
            One of these expressions must be valid:
                state.on_exit(event);
                state.on_exit();
            Where `event` is the event that caused the state transition.
            */
            awesm::state_opts::on_exit<>
        >;

        void on_entry(const button::push_event& event)
        {
            std::cout << "Turned off after a ";
            std::cout << event.duration_ms << " millisecond push\n";
        }

        void on_entry(const awesm::events::start& /*event*/)
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
An action is a function (or lambda) called when a state transition is performed.
*/
namespace actions
{
    /*
    One of the following expressions must be valid:
        action(machine, context, event);
        action(context, event);
        action(context);
    */
    void turn_light_off(context& ctx)
    {
        ctx.led.set_color(rgb_led::color::off);
    }

    //We can of course factorize with a template.
    template<auto Color>
    void turn_light_tpl(context& ctx)
    {
        ctx.led.set_color(Color);
    }
    constexpr auto turn_light_white = turn_light_tpl<rgb_led::color::white>;
    constexpr auto turn_light_red   = turn_light_tpl<rgb_led::color::red>;
    constexpr auto turn_light_green = turn_light_tpl<rgb_led::color::green>;
    constexpr auto turn_light_blue  = turn_light_tpl<rgb_led::color::blue>;
}

/*
A guard is a function (or lambda) called to check that a state transition can
be performed.
*/
namespace guards
{
    /*
    One of the following expressions must be valid:
        guard(machine, context, event);
        guard(context, event);
        guard(context);
    */
    bool is_long_push(context& /*ctx*/, const button::push_event& event)
    {
        return event.duration_ms > 1000;
    }

    //We can use awesm::guard and boolean operators to compose guards.
    constexpr auto is_short_push = !awesm::guard<is_long_push>;
}

using namespace states;
using namespace actions;
using namespace guards;
using button_push = button::push_event;
using awesm::any_but;

/*
This is the transition table. This is where we define the actions that must be
executed depending on the active state and the event we receive.
Basically, whenever awesm::sm::process_event() is called, AweSM iterates over
the transitions of this table until it finds a match, i.e. when:
- 'source_state' is the currently active state (or is awesm::any);
- 'event' is the type of the processed event;
- and the 'guard' returns true (or is void).
When a match is found, AweSM:
- exits 'source_state';
- marks 'target_state' as the new active state;
- executes the 'action';
- enters 'target_state'.
The initial active state of the state machine is the first state encountered in
the transition table ('off', is our case).
*/
using sm_transition_table = awesm::transition_table
    //    source_state,   event,       target_state,   action,           guard
    ::add<off,            button_push, emitting_white, turn_light_white>
    ::add<emitting_white, button_push, emitting_red,   turn_light_red,   is_short_push>
    ::add<emitting_red,   button_push, emitting_green, turn_light_green, is_short_push>
    ::add<emitting_green, button_push, emitting_blue,  turn_light_blue,  is_short_push>
    ::add<emitting_blue,  button_push, emitting_white, turn_light_white, is_short_push>
    ::add<any_but<off>,   button_push, off,            turn_light_off,   is_long_push>
;

/*
We have to define this struct to define our state machine. Here we just specify
the transition table, but we can put many options in it.
*/
struct sm_def
{
    using conf = awesm::sm_conf
    <
        awesm::sm_opts::transition_tables<sm_transition_table>,
        awesm::sm_opts::context<context>
    >;
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
    When we instantiate the state machine, we also instantiate:
    - a context;
    - the states mentionned in the transition table.
    Note that the states are instantiated once and for all: no construction or
    destruction happens during state transitions.
    */
    auto sm = sm_t{};
    auto& ctx = sm.get_context();

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
