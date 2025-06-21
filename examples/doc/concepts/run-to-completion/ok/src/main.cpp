//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include <iostream>

struct E0{};
struct E1{};

struct context{};

void say_hello_and_emit_E1(const maki::machine_ref_e<E1> mach)
{
    std::cout << "Hello,";
    mach.process_event(E1{});
}

void say_world()
{
    std::cout << " World!";
}

constexpr auto state_a = maki::state_mold{}
    .exit_action_m(&say_hello_and_emit_E1)
;

constexpr auto state_b = maki::state_mold{}
    .internal_action_v<E1>(&say_world)
;

constexpr auto transition_table = maki::transition_table{}
    (maki::ini, state_a)
    (state_a,   state_b, maki::event<E0>)
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
    machine.process_event(E0{});
    std::cout << "\n";
}
