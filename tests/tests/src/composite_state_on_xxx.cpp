//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"
#include <string>

namespace composite_state_on_xxx_ns
{
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

        struct dummy{};
    }

    namespace states
    {
        EMPTY_STATE(off)

        constexpr auto on_0 = maki::state_mold{}
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
            .exit_action_ce
            (
                [](context& ctx, const events::button_press& event)
                {
                    ctx.out += event.data + "1";
                }
            )
        ;

        constexpr auto on_transition_table = maki::transition_table{}
            (maki::ini,    states::on_0)
            (states::on_0, maki::null, maki::event<events::dummy>)
        ;

        constexpr auto on = maki::state_mold{}
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

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini,   states::off)
        (states::off, states::on,  maki::event<events::button_press>)
        (states::on,  states::off, maki::event<events::button_press>)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("composite_state_on_xxx")
{
    using namespace composite_state_on_xxx_ns;

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
