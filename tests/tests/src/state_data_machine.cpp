//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace
{
    struct context
    {
    };

    namespace events
    {
        struct button_press{};

        struct accumulate_request
        {
            int n = 0;
        };
    }

    namespace states
    {
        EMPTY_STATE(off)
        EMPTY_STATE(on)
    }

    constexpr auto transition_table = maki::empty_transition_table
        .add_c<states::off, events::button_press, states::on>
        .add_c<states::on,  events::button_press, states::off>
    ;

    struct machine_data
    {
        int counter = 0;
    };

    struct machine_def
    {
        static constexpr auto conf = maki::machine_conf
            .transition_tables(transition_table)
            .context<context>()
            .data<machine_data>()
            .event_action_de<events::accumulate_request>
            (
                [](machine_data& data, const events::accumulate_request& event)
                {
                    data.counter += event.n;
                }
            )
        ;
    };

    using machine_t = maki::machine<machine_def>;
}

TEST_CASE("state_data_machine")
{
    auto machine = machine_t{};
    int& counter = machine.data().counter;

    machine.process_event(events::button_press{});
    REQUIRE(counter == 0);

    machine.process_event(events::accumulate_request{1});
    REQUIRE(counter == 1);

    machine.process_event(events::accumulate_request{2});
    REQUIRE(counter == 3);
}
