//Copyright Florian Goujeon 2021 - 2025.
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
constexpr auto state0 = maki::state_mold{};
constexpr auto state1 = maki::state_mold{};
//! [short-in-state]
constexpr auto state2 = maki::state_mold{}
    /*
    Entry action.
    Called on state entry for state transitions caused by some_other_event.
    */
    .entry_action_e<some_other_event>
    (
        [](const some_other_event& event)
        {
            std::cout << "Executing state2 entry action (some_other_event{" << event.value << "})...\n";
        }
    )

    /*
    Entry action.
    Called on state entry for state transitions caused by any other type of
    event.
    */
    .entry_action_v([]
    {
        std::cout << "Executing state2 entry action...\n";
    })

    /*
    Internal action.
    */
    .internal_action_v<some_event>([]
    {
        std::cout << "Executing state2 some_event action\n";
    })

    /*
    Internal action.
    */
    .internal_action_e<some_other_event>([](const some_other_event& event)
    {
        std::cout << "Executing state2 some_other_event action (some_other_event{" << event.value << "})...\n";
    })

    /*
    Exit action.
    Called on state exit, whatever the event that caused the state
    transitions.
    */
    .exit_action_v([]
    {
        std::cout << "Executing state2 exit action...\n";
    })
;
//! [short-in-state]

//! [short-in-transition]
//Actions
constexpr auto some_action = maki::action_v([]
{
    std::cout << "Executing some action...\n";
});
constexpr auto some_other_action = maki::action_e([](const some_other_event& event)
{
    std::cout << "Executing some other action (some_other_event{" << event.value << "})...\n";
});

//Transition table
constexpr auto transition_table = maki::transition_table{}
    //source,   target,     event,                         action
    (maki::ini, state0)
    (state0,    state1,     maki::event<some_event>,       some_action /*state transition action*/)
    (state0,    maki::null, maki::event<some_other_event>, some_other_action /*internal transition action*/)
    (state0,    state2,     maki::event<yet_another_event>)
    (state1,    state2,     maki::event<yet_another_event>)
    (state2,    state0,     maki::event<yet_another_event>)
;
//! [short-in-transition]

//State machine configuration
constexpr auto machine_conf = maki::machine_conf{}
    .transition_tables(transition_table)
    .context_a<context>()
;

//State machine
using machine_t = maki::machine<machine_conf>;

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
