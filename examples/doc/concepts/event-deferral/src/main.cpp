//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

//! [all]
#include <maki.hpp>
#include <iostream>

struct context{};

//Events
struct start_button_press{};
struct end_of_initialization{};
struct play_button_press{};
struct stop_button_press{};
struct pause_button_press{};

//States
constexpr auto stopped = maki::state_mold{};
constexpr auto paused = maki::state_mold{};
constexpr auto playing = maki::state_mold{};

//! [state-mold]
constexpr auto initializing = maki::state_mold{}
    .defer<play_button_press>();
//! [state-mold]

//Transition table
//! [transition-table]
constexpr auto transition_table = maki::transition_table{}
    //source,      target,       event, action, guard
    (maki::ini,    stopped)
    (stopped,      initializing, maki::event<start_button_press>)
    (initializing, paused,       maki::event<end_of_initialization>)
    (paused,       playing,      maki::event<play_button_press>)
    (paused,       stopped,      maki::event<stop_button_press>)
    (playing,      paused,       maki::event<pause_button_press>)
    (playing,      stopped,      maki::event<stop_button_press>)
;
//! [transition-table]

constexpr auto machine_conf = maki::machine_conf{}
    .transition_tables(transition_table)
    .context_a<context>()
;

//State machine
using machine_t = maki::machine<machine_conf>;

int main()
{
    auto machine = machine_t{};

    machine.process_event(start_button_press{});

    machine.process_event(play_button_press{});
    if (machine.is<initializing>())
    {
        std::cout << "Initialing\n";
    }

    machine.process_event(end_of_initialization{});
    if (machine.is<playing>())
    {
        std::cout << "Playing\n";
    }

    return 0;
}
//! [all]
