//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"
#include <string>

namespace internal_transition_in_transition_table_ns
{
    struct context
    {
        std::string out;
    };

    namespace events
    {
        struct power_button_press{};
        struct beep_button_press{};
    }

    namespace states
    {
        constexpr auto idle = maki::state_conf{}
            .entry_action_c
            (
                maki::any,
                [](context& ctx)
                {
                    ctx.out += "idle::on_entry;";
                }
            )
            .exit_action_c
            (
                maki::any,
                [](context& ctx)
                {
                    ctx.out += "idle::on_exit;";
                }
            )
        ;

        constexpr auto running = maki::state_conf{}
            .entry_action_c
            (
                maki::any,
                [](context& ctx)
                {
                    ctx.out += "running::on_entry;";
                }
            )
            .exit_action_c
            (
                maki::any,
                [](context& ctx)
                {
                    ctx.out += "running::on_exit;";
                }
            )
        ;
    }

    namespace actions
    {
        constexpr auto beep = maki::action_c([](context& ctx)
        {
            ctx.out += "beep;";
        });
    }

    constexpr auto transition_table = maki::transition_table{}
        (states::idle,    maki::type<events::power_button_press>, states::running)
        (states::running, maki::type<events::power_button_press>, states::idle)
        (states::running, maki::type<events::beep_button_press>,  maki::null, actions::beep)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a(maki::type<context>)
    ;

    using machine_t = maki::make_machine<machine_conf>;
}

TEST_CASE("internal_transition_in_transition_table")
{
    using namespace internal_transition_in_transition_table_ns;

    auto machine = machine_t{};
    auto& ctx = machine.context();

    machine.start();
    REQUIRE(ctx.out == "idle::on_entry;");

    ctx.out.clear();
    machine.process_event(events::power_button_press{});
    REQUIRE(ctx.out == "idle::on_exit;running::on_entry;");

    ctx.out.clear();
    machine.process_event(events::beep_button_press{});
    REQUIRE(ctx.out == "beep;");
}
