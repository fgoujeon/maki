//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include <iostream>

struct beep_request{};
struct boop_request{};
struct bap_request{};

struct context{};

// [event-set]
constexpr auto busy = maki::state_conf{}
    .internal_action_v
    (
        maki::event<beep_request> || maki::event<boop_request> || maki::event<bap_request>,
        []
        {
            std::cout << "Command unavailable\n";
        }
    )
;
// [event-set]

constexpr auto transition_table = maki::transition_table{}
    (busy, maki::null, maki::no_event)
;

constexpr auto machine_conf = maki::machine_conf{}
    .transition_tables(transition_table)
    .context_a<context>()
;

using machine_t = maki::machine<machine_conf>;

int main()
{
    auto machine = machine_t{};

    machine.process_event(beep_request{});

    return 0;
}
