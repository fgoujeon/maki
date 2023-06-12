//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

//! [all]
#include <awesm.hpp>
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
struct reading_memory { using conf = awesm::state_conf; };
struct spinning_low { using conf = awesm::state_conf; };
struct spinning_med { using conf = awesm::state_conf; };
struct spinning_high { using conf = awesm::state_conf; };

//Actions
void set_speed_low()
{
    std::cout << "Speed is low\n";

    //Set fan speed and save speed in memory
    //...
}
void set_speed_med()
{
    std::cout << "Speed is med\n";

    //Set fan speed and save speed in memory
    //...
}
void set_speed_high()
{
    std::cout << "Speed is high\n";

    //Set fan speed and save speed in memory
    //...
}

//Guards
//! [guards]
bool is_speed_low(context& /*ctx*/, const memory_read& event)
{
    return event.spd == speed::low;
}
bool is_speed_med(context& /*ctx*/, const memory_read& event)
{
    return event.spd == speed::med;
}
bool is_speed_high(context& /*ctx*/, const memory_read& event)
{
    return event.spd == speed::high;
}
//! [guards]

//Transition table
//! [transition-table]
using sm_transition_table_t = awesm::transition_table
    //    source state,   event,              target state,  action,         guard
    ::add<reading_memory, memory_read,        spinning_low,  set_speed_low,  is_speed_low>
    ::add<reading_memory, memory_read,        spinning_med,  set_speed_med,  is_speed_med>
    ::add<reading_memory, memory_read,        spinning_high, set_speed_high, is_speed_high>
    ::add<spinning_low,   plus_button_press,  spinning_med,  set_speed_med>
    ::add<spinning_med,   plus_button_press,  spinning_high, set_speed_high>
    ::add<spinning_med,   minus_button_press, spinning_low,  set_speed_low>
    ::add<spinning_high,  minus_button_press, spinning_med,  set_speed_med>
;
//! [transition-table]

//State machine definition
struct sm_def
{
    //The configuration of the state machine
    using conf = awesm::sm_conf
        ::transition_tables<sm_transition_table_t>
        ::context<context>
    ;
};

//State machine
using sm_t = awesm::sm<sm_def>;

int main()
{
    auto sm = sm_t{};

    //Simulate a memory read that returns a "med" speed.
    //This eventually sets the fan speed to "med".
    sm.process_event(memory_read{speed::med});

    //Simulate button presses
    {
        //Set fan speed to "high"
        sm.process_event(plus_button_press{});

        //Doesn't do anything, as the highest possible speed is already reached
        sm.process_event(plus_button_press{});
        sm.process_event(plus_button_press{});

        //Set fan speed to "med"
        sm.process_event(minus_button_press{});

        //Set fan speed to "low"
        sm.process_event(minus_button_press{});

        //Doesn't do anything, as the lowest possible speed is already reached
        sm.process_event(minus_button_press{});
    }

    return 0;
}
//! [all]
