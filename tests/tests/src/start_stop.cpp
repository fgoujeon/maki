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
        struct s0
        {
            static constexpr auto conf = maki::default_state_conf
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
        };

        struct s1
        {
            static constexpr auto conf = maki::default_state_conf
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
        };
    }

    namespace events
    {
        struct button_press{};
    }

    constexpr auto transition_table = maki::empty_transition_table
        .add_c<states::s0, maki::null,           states::s1>
        .add_c<states::s1, events::button_press, states::s0>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::default_machine_conf
            .set_transition_tables(transition_table)
            .set_context<context>()
            .disable_auto_start()
        ;
    };

    using machine_t = maki::machine<machine_def>;
}

TEST_CASE("start_stop")
{
    auto machine = machine_t{};
    auto& ctx = machine.context();

    REQUIRE(!machine.is_running());
    REQUIRE(ctx.out == "");

    machine.start();
    REQUIRE(machine.is_active_state<states::s1>());
    REQUIRE(ctx.out == "s0::on_entry;s0::on_exit;s1::on_entry;");

    ctx.out.clear();
    machine.stop();
    REQUIRE(!machine.is_running());
    REQUIRE(ctx.out == "s1::on_exit;");
}
