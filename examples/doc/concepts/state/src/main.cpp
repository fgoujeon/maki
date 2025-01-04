//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include <iostream>

struct some_event{};

struct context{};

//! [state-conf-default]
constexpr auto my_state = maki::state_conf{};
//! [state-conf-default]

//! [state-conf-with-options]
constexpr auto my_state_with_options = maki::state_conf{}
    .entry_action_v([]{std::cout << "Hello\n";})
    .exit_action_v([]{std::cout << "Goodbye\n";});
//! [state-conf-with-options]

constexpr auto transition_table = maki::transition_table{}
    //source state,         event,                  target state
    (my_state,              maki::type<some_event>, my_state_with_options)
    (my_state_with_options, maki::type<some_event>, my_state)
;

constexpr auto machine_conf = maki::machine_conf{}
    .transition_tables(transition_table)
    .context_a<context>()
;

using machine_t = maki::machine<machine_conf>;

int main()
{
    auto machine = machine_t{};
    machine.process_event(some_event{});
    machine.process_event(some_event{});
}
