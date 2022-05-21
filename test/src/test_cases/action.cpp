//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include <catch2/catch.hpp>

namespace
{
    struct context
    {
        int i = 0;
    };

    namespace events
    {
        struct button_press{};
    }

    namespace states
    {
        struct off{};
        struct on{};
    }

    namespace actions
    {
        struct beep
        {
            void execute(const events::button_press&)
            {
                ctx.i = 1;
            }

            context& ctx;
        };

        struct boop
        {
            void execute(const states::on&, const events::button_press&, const states::off&)
            {
                ctx.i = 0;
            }

            context& ctx;
        };
    }

    struct sm_conf: awesm::sm_configuration
    {
        using transition_table = awesm::transition_table
        <
            awesm::row<states::off, events::button_press, states::on,  actions::beep>,
            awesm::row<states::on,  events::button_press, states::off, actions::boop>
        >;
    };

    using sm = awesm::sm<sm_conf>;
}

TEST_CASE("action")
{
    auto ctx = context{};
    auto machine = sm{ctx};

    machine.process_event(events::button_press{});
    REQUIRE(machine.is_active_state<states::on>());
    REQUIRE(ctx.i == 1);

    machine.process_event(events::button_press{});
    REQUIRE(machine.is_active_state<states::off>());
    REQUIRE(ctx.i == 0);
}
