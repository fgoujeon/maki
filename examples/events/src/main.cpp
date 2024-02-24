//Copyright Florian Goujeon 2021 - 2024.
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
constexpr auto idle = maki::state_conf{};
constexpr auto starting = maki::state_conf{};
constexpr auto running = maki::state_conf{};
constexpr auto stopping = maki::state_conf{};

//Actions
void start_motor(context& ctx)
{
    ctx.mtr.async_start();
}
void stop_motor(context& ctx)
{
    ctx.mtr.async_stop();
}

//Transition table
constexpr auto transition_table = maki::transition_table{}
    //source state,    event,                                     target state, action
    (idle,             maki::type<user_interface::start_request>, starting,     start_motor)
    (starting,         maki::type<motor::start_event>,            running)
    (running,          maki::type<user_interface::stop_request>,  stopping,     stop_motor)
    (stopping,         maki::type<motor::stop_event>,             idle)
;

//State machine configuration
constexpr auto machine_conf = maki::machine_conf{}
    .transition_tables(transition_table)
    .context_am(maki::type<context>)
;

//State machine
using machine_t = maki::make_machine<machine_conf>;

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
    if(machine.active_state<idle>())
    {
        std::cout << "1\n";
    }

    machine.process_event(user_interface::start_request{});
    if(machine.active_state<starting>())
    {
        std::cout << "2\n";
    }

    machine.process_event(motor::start_event{});
    if(machine.active_state<running>())
    {
        std::cout << "3\n";
    }

    machine.process_event(user_interface::stop_request{});
    if(machine.active_state<stopping>())
    {
        std::cout << "4\n";
    }

    machine.process_event(motor::stop_event{});
    if(machine.active_state<idle>())
    {
        std::cout << "5\n";
    }
}
