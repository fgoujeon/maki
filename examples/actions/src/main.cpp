//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

//! [all]
#include <maki.hpp>
#include <iostream>

struct context{};

//Events
struct some_event{};
struct some_other_event
{
    int value = 0;
};
struct yet_another_event{};

//States
constexpr auto state0 = maki::state_conf;
constexpr auto state1 = maki::state_conf;
//! [short-in-state]
constexpr auto state2 = maki::state_conf
    //Entry action.
    //Called on state entry for state transitions caused by some_other_event.
    .entry_action_e<some_other_event>([](const some_other_event& event)
    {
        std::cout << "Executing state2 entry action (some_other_event{" << event.value << "})...\n";
    })

    //Entry action.
    //Called on state entry for state transitions caused by any other type of
    //event.
    .entry_action_v<maki::any_t>([]
    {
        std::cout << "Executing state2 entry action...\n";
    })

    //Internal action.
    .internal_action_v<some_event>([]
    {
        std::cout << "Executing state2 some_event action\n";
    })

    //Internal action.
    .internal_action_e<some_other_event>([](const some_other_event& event)
    {
        std::cout << "Executing state2 some_other_event action (some_other_event{" << event.value << "})...\n";
    })

    //Exit action.
    //Called on state exit, whatever the event that caused the state
    //transitions.
    .exit_action_v<maki::any_t>([]
    {
        std::cout << "Executing state2 exit action...\n";
    })
;
//! [short-in-state]

//! [short-in-transition]
//Actions
void some_action()
{
    std::cout << "Executing some action...\n";
}
void some_other_action(context& /*ctx*/, const some_other_event& event)
{
    std::cout << "Executing some other action (some_other_event{" << event.value << "})...\n";
}

//Transition table
constexpr auto transition_table = maki::transition_table
    //     source state, event,             target state, action
    .add_c<state0,       some_event,        state1,       some_action /*state transition action*/>
    .add_c<state0,       some_other_event,  maki::null,   some_other_action /*internal transition action*/>
    .add_c<state0,       yet_another_event, state2>
    .add_c<state1,       yet_another_event, state2>
    .add_c<state2,       yet_another_event, state0>
;
//! [short-in-transition]

//State machine configuration
struct machine_conf_holder
{
    //The configuration of the state machine
    static constexpr auto conf = maki::machine_conf
        .transition_tables(transition_table)
        .context<context>()
    ;
};

//State machine
using machine_t = maki::machine<machine_conf_holder>;

int main()
{
    auto machine = machine_t{};

    //Calls some_other_action()
    machine.process_event(some_other_event{42});

    //Exits state0, calls some_action() and enters state1
    machine.process_event(some_event{});

    //No effect, because no action is associated to state1 and some_event
    machine.process_event(some_event{});

    //Exits state1 and enters state2 (which calls state2::on_entry())
    machine.process_event(yet_another_event{});

    //Calls state2::on_event(some_other_event{123})
    machine.process_event(some_other_event{1337});

    //Exits state2 (which calls state2::on_exit()) and enter state0
    machine.process_event(yet_another_event{});

    return 0;
}
//! [all]
