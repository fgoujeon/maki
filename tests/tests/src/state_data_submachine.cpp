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
        EMPTY_STATE(off);
        EMPTY_STATE(on0);
        EMPTY_STATE(on1);

        constexpr auto on_transition_table = maki::empty_transition_table
            .add_c<states::on0, maki::null, states::on1>
        ;

        struct on;

        void on_accumulate(on& self, const int n);

        struct on
        {
            static constexpr auto conf = maki::submachine_conf_c<on>
                .set_transition_tables(on_transition_table)
                .event_action_de<events::accumulate_request>
                (
                    [](on& self, const events::accumulate_request& event)
                    {
                        on_accumulate(self, event.n);
                    }
                )
            ;

            int counter = 0;
        };

        void on_accumulate(on& self, const int n)
        {
            self.counter += n;
        }
    }

    constexpr auto transition_table = maki::empty_transition_table
        .add_c<states::off, events::button_press, states::on>
        .add_c<states::on,  events::button_press, states::off>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::default_machine_conf
            .set_transition_tables(transition_table)
            .set_context<context>()
        ;
    };

    using machine_t = maki::machine<machine_def>;
}

TEST_CASE("state_data_submachine")
{
    auto machine = machine_t{};
    auto& counter = machine.state_data<maki::region_path_c<machine_def>, states::on>().counter;

    machine.process_event(events::button_press{});
    REQUIRE(counter == 0);

    machine.process_event(events::accumulate_request{1});
    REQUIRE(counter == 1);

    machine.process_event(events::accumulate_request{2});
    REQUIRE(counter == 3);
}
