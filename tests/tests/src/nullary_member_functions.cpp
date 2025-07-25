//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"
#include <string>

namespace nullary_member_functions_ns
{
    struct context
    {
        mutable std::string out;
    };

    namespace events
    {
        struct e1
        {
        };

        struct e2
        {
        };
    }

    namespace states
    {
        EMPTY_STATE(off)

        constexpr auto on = maki::state_mold{}
            .entry_action_c<events::e1>
            (
                [](context& ctx)
                {
                    ctx.out += "on_entry(e1);";
                }
            )
            .entry_action_c
            (
                [](context& ctx)
                {
                    ctx.out += "on_entry();";
                }
            )
            .exit_action_c<events::e1>
            (
                [](context& ctx)
                {
                    ctx.out += "on_exit(e1);";
                }
            )
            .exit_action_c
            (
                [](context& ctx)
                {
                    ctx.out += "on_exit();";
                }
            )
        ;
    }

    constexpr auto action = maki::action_ce([](context& ctx, const auto& event)
    {
        using event_type = std::decay_t<decltype(event)>;
        if constexpr(std::is_same_v<event_type, events::e1>)
        {
            ctx.out += "execute(e1);";
        }
        else
        {
            ctx.out += "execute();";
        }
    });

    constexpr auto guard = maki::guard_ce([](const context& ctx, const auto& event)
    {
        using event_type = std::decay_t<decltype(event)>;
        if constexpr(std::is_same_v<event_type, events::e1>)
        {
            ctx.out += "check(e1);";
        }
        else
        {
            ctx.out += "check();";
        }
        return true;
    });

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini,   states::off)
        (states::off, states::on, maki::event<events::e1>,  action, guard)
        (states::off, states::on, maki::event<events::e2>,  action, guard)
        (states::on,  states::off, maki::event<events::e1>, action, guard)
        (states::on,  states::off, maki::event<events::e2>, action, guard)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("nullary_member_functions")
{
    using namespace nullary_member_functions_ns;

    auto machine = machine_t{};
    auto& ctx = machine.context();

    machine.start();

    ctx.out.clear();
    machine.process_event(events::e1{});
    REQUIRE(ctx.out == "check(e1);execute(e1);on_entry(e1);");

    ctx.out.clear();
    machine.process_event(events::e1{});
    REQUIRE(ctx.out == "check(e1);on_exit(e1);execute(e1);");

    ctx.out.clear();
    machine.process_event(events::e2{});
    REQUIRE(ctx.out == "check();execute();on_entry();");

    ctx.out.clear();
    machine.process_event(events::e2{});
    REQUIRE(ctx.out == "check();on_exit();execute();");
}
