//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

//! [all]
#include <awesm.hpp>
#include <iostream>

//The context (more about that later)
struct context{};

//Events are types (more about that later)
struct button_press{};

//States are types (more about that later)
struct off { using conf = awesm::state_conf; };
struct on { using conf = awesm::state_conf; };

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
using sm_transition_table_t = awesm::transition_table
    //    source state, event,        target state, action
    ::add<off,          button_press, on,           turn_light_on>
    ::add<on,           button_press, off,          turn_light_off>
;
//! [transition-table]

//The definition of the state machine
//! [sm-def]
struct sm_def
{
    //The configuration of the state machine
    using conf = awesm::sm_conf
        ::transition_tables<sm_transition_table_t>
        ::context<context>
    ;
};
//! [sm-def]

//The state machine
//! [sm]
using sm_t = awesm::sm<sm_def>;
//! [sm]

int main()
{
    //Instantiation of the state machine.
    //Initial state is `off`.
    auto sm = sm_t{};

    //Process a button press event. This will call turn_light_on() and activate
    //the `on` state.
    sm.process_event(button_press{});

    //Process a button press event again. This will call turn_light_off() and
    //activate the `off` state.
    sm.process_event(button_press{});

    return 0;
}
//! [all]
