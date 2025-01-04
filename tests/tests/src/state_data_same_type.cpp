//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace state_data_same_type_ns
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
        struct data
        {
            int counter = 0;
        };

        void accumulate(data& dat, const events::accumulate_request& req)
        {
            dat.counter += req.n;
        }

        constexpr auto off = maki::state_conf{}
            .context_v<data>()
            .internal_action_ce(maki::type<events::accumulate_request>, &accumulate)
        ;

        constexpr auto on = maki::state_conf{}
            .context_v<data>()
            .internal_action_ce(maki::type<events::accumulate_request>, &accumulate)
        ;
    }

    constexpr auto transition_table = maki::transition_table{}
        (states::off, maki::type<events::button_press>, states::on)
        (states::on,  maki::type<events::button_press>, states::off)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("state_data_same_type")
{
    using namespace state_data_same_type_ns;

    auto machine = machine_t{};
    const auto off_state = machine.state<states::off>();
    const auto on_state = machine.state<states::on>();

    auto& off_counter = off_state.context().counter;
    auto& on_counter = on_state.context().counter;

    REQUIRE(off_counter == 0);
    REQUIRE(on_counter == 0);

    machine.process_event(events::accumulate_request{1});
    REQUIRE(off_counter == 1);
    REQUIRE(on_counter == 0);

    machine.process_event(events::button_press{});
    REQUIRE(off_counter == 1);
    REQUIRE(on_counter == 0);

    machine.process_event(events::accumulate_request{1});
    REQUIRE(on_counter == 1);

    machine.process_event(events::accumulate_request{2});
    REQUIRE(on_counter == 3);
}
