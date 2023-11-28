//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

//! [all]
#include <maki.hpp>
#include <iostream>

//The context (more about that later)
struct context{};

//Events are types (more about that later)
struct button_press{};

//States are types (more about that later)
constexpr auto off = maki::state_conf_c;
constexpr auto on = maki::state_conf_c;

//Actions are functions (more about that later)
void turn_light_on()
{
    std::cout << "Light is on\n";
}
void turn_light_off()
{
    std::cout << "Light is off\n";
}

//The transition table
//! [transition-table]
constexpr auto transition_table = maki::empty_transition_table
    //     source state, event,        target state, action
    .add_c<off,          button_press, on,           turn_light_on>
    .add_c<on,           button_press, off,          turn_light_off>
;
//! [transition-table]

//The definition of the state machine
//! [machine-def]
struct machine_def
{
    //The configuration of the state machine
    static constexpr auto conf = maki::default_machine_conf
        .set_transition_tables(transition_table)
        .set_context<context>()
    ;
};
//! [machine-def]

//The state machine
//! [machine]
using machine_t = maki::machine<machine_def>;
//! [machine]

int main()
{
    //Instantiation of the state machine.
    //Initial state is `off`.
    auto machine = machine_t{};

    //Process a button press event. This will call turn_light_on() and activate
    //the `on` state.
    machine.process_event(button_press{});

    //Process a button press event again. This will call turn_light_off() and
    //activate the `off` state.
    machine.process_event(button_press{});

    return 0;
}
//! [all]
