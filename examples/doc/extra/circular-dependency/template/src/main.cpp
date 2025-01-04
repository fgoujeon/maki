//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

//! [all]
#include <maki.hpp>
#include <iostream>

struct some_event{};
struct some_other_event{};

struct context{};

//! [resolution]
constexpr auto my_action = maki::action_m([](auto& mach)
{
    mach.process_event(some_other_event{});
});
//! [resolution]

constexpr auto my_state = maki::state_conf{};

constexpr auto my_other_state = maki::state_conf{};

constexpr auto transition_table = maki::transition_table{}
    (my_state, maki::type<some_event>, maki::null, my_action)
    (my_state, maki::type<some_other_event>, my_other_state)
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
