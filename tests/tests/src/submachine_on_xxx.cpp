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
    struct machine_def;
    using machine_t = maki::machine<machine_def>;

    struct context
    {
        std::string out;
    };

    namespace events
    {
        struct button_press
        {
            std::string data;
        };

        struct internal
        {
            std::string data;
        };
    }

    namespace states
    {
        EMPTY_STATE(off)

        constexpr auto on_0 = maki::state_conf
            .entry_action_ce<events::button_press>
            (
                [](context& ctx, const events::button_press& event)
                {
                    ctx.out += event.data + "2";
                }
            )
            .internal_action_ce<events::internal>
            (
                [](context& ctx, const events::internal& event)
                {
                    ctx.out += event.data + "2";
                }
            )
            .exit_action_ce<events::button_press>
            (
                [](context& ctx, const events::button_press& event)
                {
                    ctx.out += event.data + "1";
                }
            )
        ;

        constexpr auto on_transition_table = maki::empty_transition_table
            .add_c<states::on_0, events::button_press, maki::null>
        ;

        constexpr auto on = maki::submachine_conf
            .transition_tables(on_transition_table)
            .entry_action_ce<events::button_press>
            (
                [](context& ctx, const events::button_press& event)
                {
                    ctx.out += event.data + "1";
                }
            )
            .internal_action_ce<events::internal>
            (
                [](context& ctx, const events::internal& event)
                {
                    ctx.out += event.data + "1";
                }
            )
            .exit_action_ce<events::button_press>
            (
                [](context& ctx, const events::button_press& event)
                {
                    ctx.out += event.data + "2";
                }
            )
        ;
    }

    constexpr auto transition_table = maki::empty_transition_table
        .add_c<states::off, events::button_press, states::on>
        .add_c<states::on,  events::button_press, states::off>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::default_machine_conf
            .transition_tables(transition_table)
            .context<context>()
        ;
    };
}

TEST_CASE("submachine_on_xxx")
{
    auto machine = machine_t{};
    auto& ctx = machine.context();

    machine.start();

    machine.process_event(events::button_press{"a"});
    REQUIRE(ctx.out == "a1a2");

    ctx.out.clear();
    machine.process_event(events::internal{"b"});
    REQUIRE(ctx.out == "b1b2");

    ctx.out.clear();
    machine.process_event(events::button_press{"c"});
    REQUIRE(ctx.out == "c1c2");
}
