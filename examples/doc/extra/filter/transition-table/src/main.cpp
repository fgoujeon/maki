//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

//! [all]
#include <maki.hpp>
#include <memory>
#include <iostream>

struct context{};

constexpr auto initializing = maki::state_conf{};
constexpr auto running = maki::state_conf{};
constexpr auto sleeping = maki::state_conf{};
constexpr auto powering_off = maki::state_conf{};

struct end_of_init{};
struct sleep_button_press{};
struct power_button_press{};
struct kill_all_humans_button_press{};

//! [transition-table]
constexpr auto transition_table = maki::transition_table{}
    //source state,               event,                                  target state
    (initializing,                maki::type<end_of_init>,                running)
    (running,                     maki::type<sleep_button_press>,         sleeping)
    (sleeping,                    maki::any_type_but<power_button_press>, running)
    (maki::any_but(powering_off), maki::type<power_button_press>,         powering_off)
;
//! [transition-table]

constexpr auto machine_conf = maki::machine_conf{}
    .context_a(maki::type<context>)
    .transition_tables(transition_table)
;
using machine_t = maki::make_machine<machine_conf>;

void assert_true(const bool b)
{
    if (!b)
    {
        throw std::runtime_error{"Error"};
    }
}

int main()
{
    auto machine = machine_t{};

    try
    {
        assert_true(machine.is<initializing>());

        machine.process_event(end_of_init{});
        assert_true(machine.is<running>());

        machine.process_event(sleep_button_press{});
        assert_true(machine.is<sleeping>());

        machine.process_event(kill_all_humans_button_press{});
        assert_true(machine.is<running>());

        machine.process_event(sleep_button_press{});
        assert_true(machine.is<sleeping>());

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
