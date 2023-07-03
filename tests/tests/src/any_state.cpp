//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include "common.hpp"

namespace
{
    struct context{};

    namespace states
    {
        EMPTY_STATE(idle);
        EMPTY_STATE(running);
        EMPTY_STATE(failed);
    }

    namespace events
    {
        struct start_button_press{};
        struct stop_button_press{};
        struct error{};
    }

    using transition_table_t = awesm::transition_table
        ::add<states::idle,    events::start_button_press, states::running>
        ::add<states::running, events::stop_button_press,  states::idle>
        ::add<states::failed,  events::stop_button_press,  states::idle>
        ::add<awesm::any,      events::error,              states::failed>
    ;

    struct machine_def
    {
        using conf = awesm::machine_conf
            ::transition_tables<transition_table_t>
            ::context<context>
        ;
    };

    using machine_t = awesm::machine<machine_def>;
}

TEST_CASE("any state")
{
    auto machine = machine_t{};

    machine.start();

    machine.process_event(events::stop_button_press{});
    machine.process_event(events::error{});
    REQUIRE(machine.is_active_state<states::failed>());

    machine.process_event(events::stop_button_press{});
    machine.process_event(events::start_button_press{});
    machine.process_event(events::error{});
    REQUIRE(machine.is_active_state<states::failed>());
}
