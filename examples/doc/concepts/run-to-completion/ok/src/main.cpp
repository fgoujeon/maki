//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include <iostream>

struct event_0{};
struct event_1{};

struct context{};

void say_hello()
{
    std::cout << "Hello";
}

void emit_event_1_and_say_world
(
    const maki::machine_ref_e<event_1> mach
)
{
    mach.process_event(event_1{});
    std::cout << ", World!";
}

void say_goodbye()
{
    std::cout << "\nGoodbye.";
}

constexpr auto state_a = maki::state_conf{}
    .exit_action_v(&say_hello)
;

constexpr auto state_b = maki::state_conf{}
    .entry_action_m(&emit_event_1_and_say_world)
    .exit_action_v(&say_goodbye)
;

constexpr auto state_c = maki::state_conf{};

constexpr auto transition_table = maki::transition_table{}
    (state_a, maki::type<event_0>, state_b)
    (state_b, maki::type<event_1>, state_c)
;

#ifdef ENABLE_RTC
constexpr auto machine_conf = maki::machine_conf{}
    .context_a<context>()
    .transition_tables(transition_table)
    .run_to_completion(true)
;
#else
//! [machine-conf]
constexpr auto machine_conf = maki::machine_conf{}
    .context_a<context>()
    .transition_tables(transition_table)
    .run_to_completion(false) //<-- Here
;
//! [machine-conf]
#endif

using machine_t = maki::machine<machine_conf>;

int main()
{
    auto machine = machine_t{};
    machine.process_event(event_0{});
    std::cout << "\n";
}
