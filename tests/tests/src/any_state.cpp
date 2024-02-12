//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace
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

    constexpr auto make_transition_table()
    {
        return maki::transition_table{}
        (states::idle,    maki::type<events::start_button_press>, states::running)
        (states::running, maki::type<events::stop_button_press>,  states::idle)
        (states::failed,  maki::type<events::stop_button_press>,  states::idle)
        (maki::any,       maki::type<events::error>,              states::failed)
        ;
    }

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(make_transition_table)
        .context_a(maki::type<context>)
    ;

    using machine_t = maki::make_machine<machine_conf>;
}

TEST_CASE("any state")
{
    auto machine = machine_t{};

    machine.start();

    machine.process_event(events::stop_button_press{});
    machine.process_event(events::error{});
    REQUIRE(machine.active_state<states::failed>());

    machine.process_event(events::stop_button_press{});
    machine.process_event(events::start_button_press{});
    machine.process_event(events::error{});
    REQUIRE(machine.active_state<states::failed>());
}
