//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

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
        Accordingly to the explanation given in the comment just above, this
        overload is called for all the types of event that don't have their
        on_entry() overload.
        Note: This is not actually needed here since button::push_event is the
        only event type that leads to the off state.
        */
        void on_entry()
        {
        }

        /*
        Whenever the FSM processes an event, it calls the on_event() function of
        the active state by passing it the event. The FSM does this call just
        before processing the event in the transition table.
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
    Also, just like state classes, guard classes must be constructible with a
    reference to the context.
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

        context& ctx;
    };

    //We can use guard operators to combine our guards.
    using is_short_push = fgfsm::not_<is_long_push>;
}

//Allow shorter names in transition table
using namespace states;
using namespace actions;
using namespace guards;
using button_push = button::push_event;

/*
This is the transition table. This is where we define the actions that must be
executed depending on the active state and the event we receive.
Basically, whenever fgfsm::fsm::process_event() is called, FGFSM iterates over
the rows of this table until it finds a match, i.e. when:
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
using transition_table = fgfsm::transition_table
<
    //         start_state,    event,       target_state,   action,            guard
    fgfsm::row<off,            button_push, emitting_white, turn_light_white>,
    fgfsm::row<emitting_white, button_push, emitting_red,   turn_light_red,    is_short_push>,
    fgfsm::row<emitting_red,   button_push, emitting_green, turn_light_green,  is_short_push>,
    fgfsm::row<emitting_green, button_push, emitting_blue,  turn_light_blue,   is_short_push>,
    fgfsm::row<emitting_blue,  button_push, emitting_white, turn_light_white,  is_short_push>,
    fgfsm::row<fgfsm::any,     button_push, off,            turn_light_off,    is_long_push>
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
