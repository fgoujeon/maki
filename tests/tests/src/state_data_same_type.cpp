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
        struct data
        {
            int counter = 0;
        };

        void accumulate(data& dat, const events::accumulate_request& req)
        {
            dat.counter += req.n;
        }

        constexpr auto off = maki::state_conf
            .data<data>()
            .internal_action_de<events::accumulate_request>(&accumulate)
        ;

        constexpr auto on = maki::state_conf
            .data<data>()
            .internal_action_de<events::accumulate_request>(&accumulate)
        ;
    }

    constexpr auto transition_table = maki::empty_transition_table
        .add_c<states::off, events::button_press, states::on>
        .add_c<states::on,  events::button_press, states::off>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::machine_conf
            .transition_tables(transition_table)
            .context<context>()
        ;
    };

    using machine_t = maki::machine<machine_def>;
}

TEST_CASE("state_data_same_type")
{
    auto machine = machine_t{};
    auto& off_counter = machine.state_data<maki::region_path_c<machine_def::conf>, states::off>().counter;
    auto& on_counter = machine.state_data<maki::region_path_c<machine_def::conf>, states::on>().counter;

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
