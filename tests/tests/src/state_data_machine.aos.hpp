//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace AOS(state_data_machine_ns)
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
        (maki::ini,   states::off)
        (states::off, states::on, maki::event<events::button_press>)
        (states::on,  states::off, maki::event<events::button_press>)
    ;

    struct machine_data
    {
    };

    struct machine_conf
    {
        static constexpr auto value = maki::machine_conf{}
            .transition_tables(transition_table)
            .context_a<context>()
            .pre_processing_hook_ce<events::accumulate_request>
            (
                [](context& data, const events::accumulate_request& event)
                {
                    data.counter += event.n;
                }
            )
            AOS_MACHINE_OPTS
        ;
    };

    using machine_t = maki::machine<machine_conf>;

    AOS_TEST_CASE("state_data_machine")
    {
        auto machine = machine_t{};
        int& counter = machine.context().counter;

#if AOS_ASYNC
        co_await machine.async_start();
#endif

        AOS_CALL machine.AOS(process_event)(events::button_press{});
        REQUIRE(counter == 0);

        AOS_CALL machine.AOS(process_event)(events::accumulate_request{1});
        REQUIRE(counter == 1);

        AOS_CALL machine.AOS(process_event)(events::accumulate_request{2});
        REQUIRE(counter == 3);
    }
}
