//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace state_data_machine_ns
{
    struct context
    {
        int counter = 0;
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

    constexpr auto transition_table = maki::transition_table{}
        (states::off, maki::type<events::button_press>, states::on)
        (states::on,  maki::type<events::button_press>, states::off)
    ;

    struct machine_data
    {
    };

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a(maki::type<context>)
        .pre_processing_hook_ce
        (
            maki::type<events::accumulate_request>,
            [](context& data, const events::accumulate_request& event)
            {
                data.counter += event.n;
            }
        )
    ;

    using machine_t = maki::make_machine<machine_conf>;
}

TEST_CASE("state_data_machine")
{
    using namespace state_data_machine_ns;

    auto machine = machine_t{};
    int& counter = machine.context().counter;

    machine.process_event(events::button_press{});
    REQUIRE(counter == 0);

    machine.process_event(events::accumulate_request{1});
    REQUIRE(counter == 1);

    machine.process_event(events::accumulate_request{2});
    REQUIRE(counter == 3);
}
