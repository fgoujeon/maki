//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include <fgfsm.hpp>
#include <catch2/catch.hpp>

namespace
{
    struct context
    {
        int i = 0;
    };

    struct events
    {
        struct button_press{};
    };

    struct states
    {
        struct off
        {
            context& ctx;
        };

        struct on
        {
            context& ctx;
        };
    };

    struct actions
    {
        struct beep
        {
            void operator()(const states::off&, const fgfsm::event_ref&, const states::on&) const
            {
                ctx.i = 1;
            }

            void operator()(const states::on&, const fgfsm::event_ref&, const states::off&) const
            {
                ctx.i = 0;
            }

            context& ctx;
        };
    };

    using transition_table = fgfsm::transition_table
    <
        fgfsm::row<states::off, events::button_press, states::on,  actions::beep>,
        fgfsm::row<states::on,  events::button_press, states::off, actions::beep>
    >;

    using fsm = fgfsm::fsm<transition_table>;
}

TEST_CASE("action")
{
    auto ctx = context{};
    auto sm = fsm{ctx};

    sm.process_event(events::button_press{});
    REQUIRE(sm.is_active_state<states::on>());
    REQUIRE(ctx.i == 1);

    sm.process_event(events::button_press{});
    REQUIRE(sm.is_active_state<states::off>());
    REQUIRE(ctx.i == 0);
}
