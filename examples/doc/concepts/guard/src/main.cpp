//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

//! [all]
#include <maki.hpp>
#include <functional>
#include <iostream>

struct context{};

//! [events-and-datatypes]
//Data types
enum class speed
{
    low,
    med,
    high
};

//Events
struct memory_read
{
    speed spd = speed::low;
};
struct minus_button_press{};
struct plus_button_press{};
//! [events-and-datatypes]

//States
constexpr auto reading_memory = maki::state_mold{};
constexpr auto spinning_low = maki::state_mold{}
    .entry_action_v([]
    {
        std::cout << "Speed is low\n";

        //Set fan speed and save speed in memory
        //[Implementation detail...]
    })
;
constexpr auto spinning_med = maki::state_mold{}
    .entry_action_v([]
    {
        std::cout << "Speed is med\n";

        //Set fan speed and save speed in memory
        //[Implementation detail...]
    })
;
constexpr auto spinning_high = maki::state_mold{}
    .entry_action_v([]
    {
        std::cout << "Speed is high\n";

        //Set fan speed and save speed in memory
        //[Implementation detail...]
    })
;

//Guards
//! [guards]
constexpr auto is_speed_low = maki::guard_e([](const memory_read& event)
{
    return event.spd == speed::low;
});
constexpr auto is_speed_med = maki::guard_e([](const memory_read& event)
{
    return event.spd == speed::med;
});
constexpr auto is_speed_high = maki::guard_e([](const memory_read& event)
{
    return event.spd == speed::high;
});
//! [guards]

//Transition table
//! [transition-table]
constexpr auto transition_table = maki::transition_table{}
    //source,        target,        event,                           action,     guard
    (maki::ini,      reading_memory)
    (reading_memory, spinning_low,  maki::event<memory_read>,        maki::null, is_speed_low)
    (reading_memory, spinning_med,  maki::event<memory_read>,        maki::null, is_speed_med)
    (reading_memory, spinning_high, maki::event<memory_read>,        maki::null, is_speed_high)
    (spinning_low,   spinning_med,  maki::event<plus_button_press>,  maki::null)
    (spinning_med,   spinning_high, maki::event<plus_button_press>,  maki::null)
    (spinning_med,   spinning_low,  maki::event<minus_button_press>, maki::null)
    (spinning_high,  spinning_med,  maki::event<minus_button_press>, maki::null)
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

    //Simulate a memory read that returns a "med" speed.
    //This eventually sets the fan speed to "med".
    machine.process_event(memory_read{speed::med});

    //Simulate button presses
    {
        //Set fan speed to "high"
        machine.process_event(plus_button_press{});

        //Doesn't do anything, as the highest possible speed is already reached
        machine.process_event(plus_button_press{});
        machine.process_event(plus_button_press{});

        //Set fan speed to "med"
        machine.process_event(minus_button_press{});

        //Set fan speed to "low"
        machine.process_event(minus_button_press{});

        //Doesn't do anything, as the lowest possible speed is already reached
        machine.process_event(minus_button_press{});
    }

    return 0;
}
//! [all]
