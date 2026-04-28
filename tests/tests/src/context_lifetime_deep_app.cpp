//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace context_lifetime_deep_app_ns
{
    struct context
    {
        int i = 0;
        int context_1_destructor_called = 0;
        int context_2_destructor_called = 0;
        int context_3_destructor_called = 0;
    };

    struct context_1
    {
        context_1(context& parent):
            parent(parent)
        {
        }

        ~context_1()
        {
            ++root.context_1_destructor_called;
        }

        context& parent;
        context& root = parent;
        int& i = parent.i;
    };

    struct context_2
    {
        context_2(context_1& parent):
            parent(parent)
        {
        }

        ~context_2()
        {
            ++root.context_2_destructor_called;
        }

        context_1& parent;
        context& root = parent.root;
        int& i = parent.i;
    };

    struct context_3
    {
        context_3(context_2& parent):
            parent(parent)
        {
        }

        ~context_3()
        {
            ++root.context_3_destructor_called;
        }

        context_2& parent;
        context& root = parent.root;
        int& i = parent.i;
    };

    struct out_event{};

    constexpr auto state_3 = maki::state_mold{}
        .context_c<context_3>()
        .entry_action_c(
            [](context_3& ctx)
            {
                ++ctx.i;
            })
    ;

    constexpr auto transition_table_2 = maki::transition_table{}
        (maki::ini, state_3)
        (state_3,   maki::fin, maki::event<out_event>)
    ;

    constexpr auto state_2 = maki::state_mold{}
        .transition_tables(transition_table_2)
        .context_c<context_2>()
    ;

    constexpr auto transition_table_1 = maki::transition_table{}
        (maki::ini, state_2)
        (state_2,   maki::fin)
    ;

    constexpr auto state_1 = maki::state_mold{}
        .transition_tables(transition_table_1)
        .context_c<context_1>()
        .context_lifetime(maki::state_context_lifetime::state_activity)
    ;

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini, state_1)
        (state_1,   maki::fin)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("context_lifetime_deep_app")
{
    using namespace context_lifetime_deep_app_ns;

    auto machine = machine_t{};
    auto& ctx = machine.context();

    machine.start();
    REQUIRE(ctx.i == 1);
    REQUIRE(ctx.context_1_destructor_called == 0);
    REQUIRE(ctx.context_2_destructor_called == 0);
    REQUIRE(ctx.context_3_destructor_called == 0);

    machine.process_event(out_event{});
    REQUIRE(ctx.context_1_destructor_called == 1);
    REQUIRE(ctx.context_2_destructor_called == 1);
    REQUIRE(ctx.context_3_destructor_called == 1);
}
