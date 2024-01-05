//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"
#include <string>

namespace
{
    struct context
    {
        std::string out;
    };

    namespace states
    {
        constexpr auto s0 = maki::state_conf{}
            .entry_action_c<maki::any>
            (
                [](context& ctx)
                {
                    ctx.out += "s0::on_entry;";
                }
            )
            .exit_action_c<maki::any>
            (
                [](context& ctx)
                {
                    ctx.out += "s0::on_exit;";
                }
            )
        ;

        constexpr auto s1 = maki::state_conf{}
            .entry_action_c<maki::any>
            (
                [](context& ctx)
                {
                    ctx.out += "s1::on_entry;";
                }
            )
            .exit_action_c<maki::any>
            (
                [](context& ctx)
                {
                    ctx.out += "s1::on_exit;";
                }
            )
        ;
    }

    namespace events
    {
        struct button_press{};
    }

    constexpr auto transition_table = maki::transition_table{}
        .add_c<states::s0, maki::null,           states::s1>
        .add_c<states::s1, events::button_press, states::s0>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::machine_conf{}
            .transition_tables(transition_table)
            .context<context>()
            .auto_start(false)
        ;
    };

    using machine_t = maki::machine<machine_def>;
}

TEST_CASE("start_stop")
{
    auto machine = machine_t{};
    auto& ctx = machine.context();

    REQUIRE(!machine.running());
    REQUIRE(ctx.out == "");

    machine.start();
    REQUIRE(machine.active_state<states::s1>());
    REQUIRE(ctx.out == "s0::on_entry;s0::on_exit;s1::on_entry;");

    ctx.out.clear();
    machine.stop();
    REQUIRE(!machine.running());
    REQUIRE(ctx.out == "s1::on_exit;");
}
