//Copyright Florian Goujeon 2021 - 2022.
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

    namespace events
    {
        struct button_press{};
    }

    namespace states
    {
        FGFSM_SIMPLE_STATE(off)
        FGFSM_SIMPLE_STATE(on)
    }

    namespace actions
    {
        template<int Value>
        struct beep
        {
            void execute(const fgfsm::any_cref&)
            {
                ctx.i = Value;
            }

            context& ctx;
        };
    }

    using transition_table = fgfsm::transition_table
    <
        fgfsm::row<states::off, events::button_press, states::on,  actions::beep<1>>,
        fgfsm::row<states::on,  events::button_press, states::off, actions::beep<0>>
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
