//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace process_event_now_ns
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
        constexpr auto s0 = maki::state_builder{}
            .entry_action_c
            (
                [](context& ctx)
                {
                    ctx.output += "s0::on_entry;";
                }
            )
            .exit_action_c
            (
                [](context& ctx)
                {
                    ctx.output += "s0::on_exit;";
                }
            )
        ;

        constexpr auto s1 = maki::state_builder{}
            .entry_action_c
            (
                [](context& ctx)
                {
                    ctx.output += "s1::on_entry;";
                }
            )
            .exit_action_c
            (
                [](context& ctx)
                {
                    ctx.output += "s1::on_exit;";
                }
            )
        ;

        constexpr auto s2 = maki::state_builder{}
            .entry_action_c
            (
                [](context& ctx)
                {
                    ctx.output += "s2::on_entry;";
                }
            )
            .exit_action_c
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
        template<class Event>
        constexpr auto process_event_now = maki::action_m([](auto& mach)
        {
            // Shouldn't have any effect as the active state is `undefined`.
            mach.process_event_now(Event{});
        });

        constexpr auto& process_s1_to_s2 = process_event_now<events::s1_to_s2_request>;
        constexpr auto& process_s2_to_s0 = process_event_now<events::s2_to_s0_request>;
    }

    constexpr auto transition_table = maki::transition_table{}
        (maki::init, states::s0)
        (states::s0, states::s1, maki::event<events::s0_to_s1_request>, actions::process_s1_to_s2)
        (states::s1, states::s2, maki::event<events::s1_to_s2_request>, actions::process_s2_to_s0)
        (states::s2, states::s0, maki::event<events::s2_to_s0_request>)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
        .process_event_now_enabled(true)
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("process_event_now")
{
    using namespace process_event_now_ns;

    auto machine = machine_t{};
    auto& ctx = machine.context();

    machine.start();
    REQUIRE(ctx.output == "s0::on_entry;");

    ctx.output.clear();
    machine.process_event_now(events::s0_to_s1_request{});
    REQUIRE
    (
        ctx.output ==
            "s0::on_exit;"
            "s1::on_entry;"
    );
}
