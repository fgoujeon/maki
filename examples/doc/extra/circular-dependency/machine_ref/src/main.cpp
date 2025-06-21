//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

//! [all]
#include <maki.hpp>
#include <iostream>

struct some_event{};
struct my_event{};
struct my_other_event{};

struct context{};

//! [resolution]
using my_machine_ref_t = maki::machine_ref_e<my_event, my_other_event>;

constexpr auto my_action = maki::action_m([](const my_machine_ref_t mach)
{
    mach.process_event(my_event{});
});

constexpr auto my_other_action = maki::action_m([](const my_machine_ref_t mach)
{
    mach.process_event(my_other_event{});
});
//! [resolution]

constexpr auto my_state = maki::state_builder{};

constexpr auto my_other_state = maki::state_builder{};

constexpr auto transition_table = maki::transition_table{}
    (maki::ini, my_state)
    (my_state,  maki::null,     maki::event<some_event>, my_action)
    (my_state,  maki::null,     maki::event<my_event>,   my_other_action)
    (my_state,  my_other_state, maki::event<my_other_event>)
;

constexpr auto machine_conf = maki::machine_conf{}
    .context_a<context>()
    .transition_tables(transition_table)
;

using machine_t = maki::machine<machine_conf>;

int main()
{
    auto machine = machine_t{};
    machine.process_event(some_event{});

    if(machine.is<my_other_state>())
    {
        std::cout << "OK\n";
    }
    else
    {
        std::cout << "NOK\n";
    }
}
//! [all]
