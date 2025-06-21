//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

//I lied muahahahahaha!
#define main fake_main

#include <iostream>

//! [all]
#include "user_interface.hpp"
#include "motor.hpp"
#include <maki.hpp>

//The context, instantiated by machine_t and accessible to all the actions and guards
struct context
{
    /*
    Note: Machine is always machine_t (defined below).
    We need a template to break circular dependency.
    */
    template<class Machine>
    context(Machine& machine):
        user_itf(make_event_callback(machine)),
        mtr(make_event_callback(machine))
    {
    }

    /*
    Forward events (from event variants) to state machine.

    Note: Machine is always machine_t (defined below).
    We need a template to break circular dependency.
    */
    template<class Machine>
    static auto make_event_callback(Machine& machine)
    {
        return [&machine](const auto& event_variant)
        {
            std::visit
            (
                [&machine](const auto& event)
                {
                    machine.process_event(event);
                },
                event_variant
            );
        };
    }

    user_interface user_itf;
    motor mtr;
};

//States
constexpr auto idle = maki::state_builder{};
constexpr auto starting = maki::state_builder{};
constexpr auto running = maki::state_builder{};
constexpr auto stopping = maki::state_builder{};

//Actions
constexpr auto start_motor = maki::action_c([](context& ctx)
{
    ctx.mtr.async_start();
});
constexpr auto stop_motor = maki::action_c([](context& ctx)
{
    ctx.mtr.async_stop();
});

//Transition table
constexpr auto transition_table = maki::transition_table{}
    //source,   target,   event,                                      action
    (maki::ini, idle)
    (idle,      starting, maki::event<user_interface::start_request>, start_motor)
    (starting,  running,  maki::event<motor::start_event>)
    (running,   stopping, maki::event<user_interface::stop_request>,  stop_motor)
    (stopping,  idle,     maki::event<motor::stop_event>)
;

//State machine configuration
constexpr auto machine_conf = maki::machine_conf{}
    .transition_tables(transition_table)
    .context_am<context>()
;

//State machine
using machine_t = maki::machine<machine_conf>;

int main()
{
    //Instantiate the state machine
    auto machine = machine_t{};

    //Run indefinitely...
    //[Implementation detail...]

    return 0;
}
//! [all]

#undef main

//We want to test this program without spamming the doc with testing details.
int main()
{
    auto machine = machine_t{};
    if(machine.is<idle>())
    {
        std::cout << "1\n";
    }

    machine.process_event(user_interface::start_request{});
    if(machine.is<starting>())
    {
        std::cout << "2\n";
    }

    machine.process_event(motor::start_event{});
    if(machine.is<running>())
    {
        std::cout << "3\n";
    }

    machine.process_event(user_interface::stop_request{});
    if(machine.is<stopping>())
    {
        std::cout << "4\n";
    }

    machine.process_event(motor::stop_event{});
    if(machine.is<idle>())
    {
        std::cout << "5\n";
    }
}
