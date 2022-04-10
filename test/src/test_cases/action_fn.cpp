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

    struct some_event
    {
        int value = 0;
    };

    struct assign_i_1
    {
        void execute(const some_event& event)
        {
            ctx.i = event.value;
        }

        context& ctx;
    };

    void assign_i_2_impl(context& ctx, const some_event& event)
    {
        ctx.i = event.value;
    }
    using assign_i_2 = fgfsm::action_fn<assign_i_2_impl>;
}

TEST_CASE("action_fn")
{
    auto ctx = context{};
    auto action_1 = assign_i_1{ctx};
    auto action_2 = assign_i_2{ctx};

    auto evt = some_event{1};

    ctx.i = 0;
    action_1.execute(evt);
    REQUIRE(ctx.i == 1);

    ctx.i = 0;
    action_2.execute(evt);
    REQUIRE(ctx.i == 1);
}
