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
        This function is called whenever the FSM enters this state.
        The event that caused the state transition is given as argument.
        You might need to write a set of overloads of on_entry() functions
        to handle all the possible event types that can lead to this state.
        */
        void on_entry(const button::push_event& event)
        {
            std::cout << "Turned off after a ";
            std::cout << event.duration_ms << " millisecond push\n";
        }

        /*
        This function is called whenever fgfsm::fsm::process_event() is called,
        provided this state is active.
        Unlike the other functions that take an event as argument, on_event()
        receives the event wrapped into an fgfsm::any_cref object.
        */
        void on_event(const fgfsm::any_cref& event)
        {
            /*
            fgfsm::any_cref is a std::any-like container that stores a reference
            to a const object. FGFSM provides visitation functions to access
            fgfsm::any_cref objects in a safe and concise way.
            For example, fgfsm::visit() takes an fgfsm::any_cref and a series of
            unary function objects. The function object whose parameter type
            matches the type of the object wrapped into the fgfsm::any_cref gets
            called.
            */
            fgfsm::visit
            (
                event,
                [&](const button::push_event& event)
                {
                    std::cout << "Received a ";
                    std::cout << event.duration_ms;
                    std::cout << " millisecond push in off state\n";
                }
            );
        }

        /*
        This function is called whenever the FSM exits this state.
        Just like with on_entry(), the event that caused the state transition is
        given as argument.
        */
        void on_exit(const button::push_event& /*event*/)
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
        void execute(const button::push_event& /*event*/)
        {
            ctx.led.set_color(rgb_led::color::off);
        }

        context& ctx;
    };

    //We can of course factorize with a template.
    template<auto Color>
    struct turn_light_tpl
    {
        void execute(const button::push_event& /*event*/)
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
Basically, whenever fgfsm::fsm::process_event(event) is called, FGFSM iterates
over the rows of this table until it finds a match, i.e. when:
- 'start state' is the currently active state (or is fgfsm::any);
- 'event' is the type of the processed event;
- and the 'guard' returns true (or is fgfsm::none).
When a match is found, FGFSM:
- calls start_state.on_exit(event);
- makes 'target state' the new active state;
- executed the 'action';
- calls target_state.on_entry(event).
The initial active state of the FSM is the first state encountered in the
transition table ('off', is our case).
*/
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
