//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

//! [all]
#include <maki.hpp>
#include <iostream>

/*
An object of this type is created by the state machine and made accessible to
all the components (actions, guards, states) of the state machine.
More about that later.
*/
struct context{};

//Events are types (more about that later)
struct button_press{};

//States are constexpr objects (more about that later)
constexpr auto off = maki::state_conf{};
constexpr auto on = maki::state_conf{};

//Actions are constexpr objects (more about that later)
constexpr auto turn_light_on = maki::action_v([]
{
    std::cout << "Light is on\n";
});
constexpr auto turn_light_off = maki::action_v([]
{
    std::cout << "Light is off\n";
});

//The transition table
//! [transition-table]
constexpr auto transition_table = maki::transition_table{}
    //source state, event,                     target state, action
    (off,           maki::event<button_press>, on,           turn_light_on)
    (on,            maki::event<button_press>, off,          turn_light_off)
;
//! [transition-table]

//The configuration of the state machine
//! [machine-def]
constexpr auto machine_conf = maki::machine_conf{}
    .transition_tables(transition_table)
    .context_a<context>()
;
//! [machine-def]

//The state machine
//! [machine]
using machine_t = maki::machine<machine_conf>;
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
