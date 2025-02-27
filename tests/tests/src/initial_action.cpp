//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace initial_action
{
    struct context
    {
        int count = 0;
    };

    namespace events
    {
        struct button_press{};
    }

    namespace states
    {
        EMPTY_STATE(off)
    }

    namespace actions
    {
        constexpr auto beep = maki::action_c([](context& ctx)
        {
            ++ctx.count;
        });
    }

    constexpr auto transition_table = maki::transition_table{}
        (maki::init, states::off, maki::null, actions::beep)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
        .auto_start(false)
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("initial_action")
{
    using namespace initial_action;

    auto machine = machine_t{};
    auto& ctx = machine.context();

    CHECK(ctx.count == 0);

    machine.start();
    CHECK(machine.is<states::off>());
    CHECK(ctx.count == 1);
}
