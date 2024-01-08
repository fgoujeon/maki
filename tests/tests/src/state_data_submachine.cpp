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
        EMPTY_STATE(on0)
        EMPTY_STATE(on1)

        constexpr auto on_transition_table = maki::transition_table{}
            .add<states::on0, maki::null, states::on1>()
        ;

        struct on_data
        {
            int counter = 0;
        };

        void on_accumulate(on_data& self, const int n)
        {
            self.counter += n;
        }

        constexpr auto on = maki::submachine_conf{}
            .data<on_data>()
            .transition_tables(on_transition_table)
            .internal_action_de<events::accumulate_request>
            (
                [](on_data& self, const events::accumulate_request& event)
                {
                    on_accumulate(self, event.n);
                }
            )
        ;
    }

    constexpr auto transition_table = maki::transition_table{}
        .add<states::off, events::button_press, states::on>()
        .add<states::on,  events::button_press, states::off>()
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context<context>()
    ;

    using machine_t = maki::make_machine<machine_conf>;
}

TEST_CASE("state_data_submachine")
{
    auto machine = machine_t{};
    static constexpr auto region_0_path = maki::path{0};
    static constexpr auto on_path = region_0_path / states::on;
    auto& counter = machine.data<on_path>().counter;

    machine.process_event(events::button_press{});
    REQUIRE(counter == 0);

    machine.process_event(events::accumulate_request{1});
    REQUIRE(counter == 1);

    machine.process_event(events::accumulate_request{2});
    REQUIRE(counter == 3);
}
