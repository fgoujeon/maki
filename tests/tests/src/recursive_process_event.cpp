//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace
{
    struct machine_def;
    using machine_t = maki::machine<machine_def>;

    struct context
    {
        std::string output;
    };

    namespace events
    {
        struct s0_to_s1_request{};
        struct s1_to_s2_request{};
        struct s2_to_s0_request{};
    }

    namespace states
    {
        constexpr auto s0 = maki::state_conf_c
            .entry_action_c<maki::any>
            (
                [](context& ctx)
                {
                    ctx.output += "s0::on_entry;";
                }
            )
            .exit_action_c<maki::any>
            (
                [](context& ctx)
                {
                    ctx.output += "s0::on_exit;";
                }
            )
        ;

        constexpr auto s1 = maki::state_conf_c
            .entry_action_c<maki::any>
            (
                [](context& ctx)
                {
                    ctx.output += "s1::on_entry;";
                }
            )
            .exit_action_c<maki::any>
            (
                [](context& ctx)
                {
                    ctx.output += "s1::on_exit;";
                }
            )
        ;

        constexpr auto s2 = maki::state_conf_c
            .entry_action_c<maki::any>
            (
                [](context& ctx)
                {
                    ctx.output += "s2::on_entry;";
                }
            )
            .exit_action_c<maki::any>
            (
                [](context& ctx)
                {
                    ctx.output += "s2::on_exit;";
                }
            )
        ;
    }

    namespace actions
    {
        constexpr auto s0_to_s1 = []
        (
            maki::machine_ref_e<events::s1_to_s2_request> mach,
            context& /*ctx*/,
            const auto& /*event*/
        )
        {
            mach.process_event(events::s1_to_s2_request{});
        };

        constexpr auto s1_to_s2 = []
        (
            maki::machine_ref_e<events::s2_to_s0_request> mach,
            context& /*ctx*/,
            const auto& /*event*/
        )
        {
            mach.process_event(events::s2_to_s0_request{});
        };
    }

    constexpr auto transition_table = maki::transition_table_c
        .add_c<states::s0, events::s0_to_s1_request, states::s1, actions::s0_to_s1>
        .add_c<states::s1, events::s1_to_s2_request, states::s2, actions::s1_to_s2>
        .add_c<states::s2, events::s2_to_s0_request, states::s0>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::machine_conf_c
            .transition_tables(transition_table)
            .context<context>()
        ;
    };
}

TEST_CASE("recursive process_event")
{
    auto machine = machine_t{};
    auto& ctx = machine.context();

    machine.start();
    REQUIRE(ctx.output == "s0::on_entry;");

    //Indirectly process s1_to_s2_request and s2_to_s0_request
    ctx.output.clear();
    machine.process_event(events::s0_to_s1_request{});
    REQUIRE
    (
        ctx.output ==
            "s0::on_exit;"
            "s1::on_entry;"
            "s1::on_exit;"
            "s2::on_entry;"
            "s2::on_exit;"
            "s0::on_entry;"
    );
}
