//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace any_state_ns
{
    struct context{};

    namespace states
    {
        EMPTY_STATE(idle)
        EMPTY_STATE(running)
        EMPTY_STATE(failed)
    }

    namespace events
    {
        struct start_button_press{};
        struct stop_button_press{};
        struct error{};
    }

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini,        states::idle)
        (states::idle,     states::running, maki::event<events::start_button_press>)
        (states::running,  states::idle,    maki::event<events::stop_button_press>)
        (states::failed,   states::idle,    maki::event<events::stop_button_press>)
        (maki::all_states, states::failed,  maki::event<events::error>)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("any state")
{
    using namespace any_state_ns;

    auto machine = machine_t{};

    machine.start();

    machine.process_event(events::stop_button_press{});
    machine.process_event(events::error{});
    REQUIRE(machine.is<states::failed>());

    machine.process_event(events::stop_button_press{});
    machine.process_event(events::start_button_press{});
    machine.process_event(events::error{});
    REQUIRE(machine.is<states::failed>());
}
