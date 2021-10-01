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
        bool has_power = false;
    };

    namespace states
    {
        struct on
        {
            context& ctx;
        };

        struct off
        {
            context& ctx;
        };
    }

    namespace events
    {
        struct button_press{};
    }

    namespace guards
    {
        struct has_power
        {
            bool operator()
            (
                const states::off&,
                const events::button_press&,
                const states::on&
            )
            {
                return ctx.has_power;
            }

            context& ctx;
        };
    }

    using transition_table = fgfsm::transition_table
    <
        fgfsm::row<states::off, events::button_press, states::on, fgfsm::none, guards::has_power>,
        fgfsm::row<states::on,  events::button_press, states::off, fgfsm::none>
    >;

    using fsm = fgfsm::fsm<transition_table>;
}

TEST_CASE("guard")
{
    auto ctx = context{};
    auto sm = fsm{ctx};

    sm.process_event(events::button_press{});
    REQUIRE(sm.is_active_state<states::off>());

    ctx.has_power = true;
    sm.process_event(events::button_press{});
    REQUIRE(sm.is_active_state<states::on>());
}
