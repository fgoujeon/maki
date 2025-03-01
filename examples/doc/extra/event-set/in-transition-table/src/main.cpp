//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

//! [all]
#include <maki.hpp>
#include <iostream>

struct end_of_init{};
struct inactivity{};

//! [event-set]
struct on_button_press{};
struct off_button_press{};

constexpr auto power_button_press =
    maki::event<on_button_press> ||
    maki::event<off_button_press>
;
//! [event-set]

struct context{};

//States
constexpr auto off = maki::state_builder{};
constexpr auto sleeping = maki::state_builder{};
constexpr auto running = maki::state_builder{};

//Transition table
//! [transition-table]
constexpr auto transition_table = maki::transition_table{}
    //source,    target,   event
    (maki::init, off)
    (off,        running,  maki::event<on_button_press>)
    (running,    sleeping, maki::event<inactivity>)
    (sleeping,   running,  power_button_press) // <-- Here
    (running,    off,      maki::event<off_button_press>)
;
//! [transition-table]

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

    machine.process_event(on_button_press{});
    if(machine.is<running>())
    {
        std::cout << "OK\n";
    }

    machine.process_event(inactivity{});
    if(machine.is<sleeping>())
    {
        std::cout << "OK\n";
    }

    machine.process_event(inactivity{});
    if(machine.is<sleeping>())
    {
        std::cout << "OK\n";
    }

    machine.process_event(off_button_press{});
    if(machine.is<running>())
    {
        std::cout << "OK\n";
    }

    return 0;
}
//! [all]
