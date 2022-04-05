//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include <fgfsm.hpp>
#include <catch2/catch.hpp>
#include <string>

namespace
{
    struct context
    {
        int i = 0;
    };

    struct int_data
    {
        int value = 0;
    };

    struct assign_i_1
    {
        void execute(const fgfsm::any_cref& event)
        {
            fgfsm::visit
            (
                event,
                [this](const int_data& event)
                {
                    ctx.i = event.value;
                }
            );
        }

        context& ctx;
    };

    void assign_i_2_impl(context& ctx, const fgfsm::any_cref& event)
    {
        fgfsm::visit
        (
            event,
            [&](const int_data& event)
            {
                ctx.i = event.value;
            }
        );
    }
    using assign_i_2 = fgfsm::action_fn<assign_i_2_impl>;

    void assign_i_3_impl(context& ctx, const int_data& event)
    {
        ctx.i = event.value;
    }
    using assign_i_3 = fgfsm::action_fn<assign_i_3_impl>;
}

TEST_CASE("action_fn")
{
    auto ctx = context{};
    auto action_1 = assign_i_1{ctx};
    auto action_2 = assign_i_2{ctx};
    auto action_3 = assign_i_3{ctx};

    auto evt = int_data{1};
    auto evt_any = fgfsm::any_cref{evt};

    ctx.i = 0;
    action_1.execute(evt_any);
    REQUIRE(ctx.i == 1);

    ctx.i = 0;
    action_2.execute(evt_any);
    REQUIRE(ctx.i == 1);

    ctx.i = 0;
    action_3.execute(evt_any);
    REQUIRE(ctx.i == 1);
}
