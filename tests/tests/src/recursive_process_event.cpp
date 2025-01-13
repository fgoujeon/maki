//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace recursive_process_event_ns
{
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
        constexpr auto s0 = maki::state_conf{}
            .entry_action_c
            (
                maki::any_event,
                [](context& ctx)
                {
                    ctx.output += "s0::on_entry;";
                }
            )
            .exit_action_c
            (
                maki::any_event,
                [](context& ctx)
                {
                    ctx.output += "s0::on_exit;";
                }
            )
        ;

        constexpr auto s1 = maki::state_conf{}
            .entry_action_c
            (
                maki::any_event,
                [](context& ctx)
                {
                    ctx.output += "s1::on_entry;";
                }
            )
            .exit_action_c
            (
                maki::any_event,
                [](context& ctx)
                {
                    ctx.output += "s1::on_exit;";
                }
            )
        ;

        constexpr auto s2 = maki::state_conf{}
            .entry_action_c
            (
                maki::any_event,
                [](context& ctx)
                {
                    ctx.output += "s2::on_entry;";
                }
            )
            .exit_action_c
            (
                maki::any_event,
                [](context& ctx)
                {
                    ctx.output += "s2::on_exit;";
                }
            )
        ;
    }

    namespace actions
    {
        constexpr auto s0_to_s1 = maki::action_m([]
        (
            maki::machine_ref_e<events::s1_to_s2_request> mach
        )
        {
            mach.process_event(events::s1_to_s2_request{});
        });

        constexpr auto s1_to_s2 = maki::action_m([]
        (
            maki::machine_ref_e<events::s2_to_s0_request> mach
        )
        {
            mach.process_event(events::s2_to_s0_request{});
        });
    }

    constexpr auto transition_table = maki::transition_table{}
        (states::s0, maki::type<events::s0_to_s1_request>, states::s1, actions::s0_to_s1)
        (states::s1, maki::type<events::s1_to_s2_request>, states::s2, actions::s1_to_s2)
        (states::s2, maki::type<events::s2_to_s0_request>, states::s0)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("recursive_process_event")
{
    using namespace recursive_process_event_ns;

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
