//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

//! [all]
#include <maki.hpp>
#include <memory>
#include <iostream>

struct context{};

//! [state-set]
constexpr auto initializing = maki::state_conf{};
constexpr auto running = maki::state_conf{};
constexpr auto sleeping = maki::state_conf{};

constexpr auto powering_off = maki::state_conf{}
    .entry_action_v([]
    {
        std::cout << "Powering off...\n";
    })
;

//A state set
constexpr auto not_powering_off = !powering_off;
//! [state-set]

struct end_of_init{};
struct sleep_button_press{};
struct wake_up_button_press{};
struct power_button_press{};

//! [transition-table]
constexpr auto transition_table = maki::transition_table{}
    //source,       target,       event
    (maki::init,    initializing)
    (initializing,  running,      maki::event<end_of_init>)
    (running,       sleeping,     maki::event<sleep_button_press>)
    (sleeping,      running,      maki::event<wake_up_button_press>)
    (!powering_off, powering_off, maki::event<power_button_press>) // <-- Here
;
//! [transition-table]

constexpr auto machine_conf = maki::machine_conf{}
    .context_a<context>()
    .transition_tables(transition_table)
;
using machine_t = maki::machine<machine_conf>;

void assert_true(const bool b)
{
    if(!b)
    {
        throw std::runtime_error{"Error"};
    }
}

int main()
{
    auto machine = machine_t{};

    //Silence "unused variable" warning
    constexpr auto not_powering_off_2 = !powering_off;
    assert_true(&not_powering_off != &not_powering_off_2);

    try
    {
        assert_true(machine.is<initializing>());

        machine.process_event(end_of_init{});
        assert_true(machine.is<running>());

        machine.process_event(sleep_button_press{});
        assert_true(machine.is<sleeping>());

        machine.process_event(power_button_press{});
        assert_true(machine.is<powering_off>());

        machine.process_event(power_button_press{});
        assert_true(machine.is<powering_off>());

        std::cout << "OK\n";
    }
    catch (...)
    {
        std::cout << "Error\n";
    }
}
//! [all]
