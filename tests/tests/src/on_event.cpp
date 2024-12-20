//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"
#include <string>

namespace on_event_ns
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

        struct alert_button_press
        {
        };
    }

    namespace states
    {
        constexpr auto off = maki::state_conf{};

        constexpr auto on = maki::state_conf{}
            .internal_action_c
            (
                maki::type<events::button_press>,
                [](context& ctx)
                {
                    ctx.out += "_";
                }
            )
            .internal_action_c
            (
                maki::type<events::alert_button_press>,
                [](context& ctx)
                {
                    ctx.out += "beep;";
                }
            )
        ;
    }

    constexpr auto transition_table = maki::transition_table{}
        (states::off, maki::type<events::button_press>, states::on)
        (states::on,  maki::type<events::button_press>, states::off)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a(maki::type<context>)
        .pre_processing_hook_ce
        (
            maki::type<events::button_press>,
            [](context& ctx, const events::button_press& event)
            {
                ctx.out += event.data + "1;";
            }
        )
    ;

    using machine_t = maki::make_machine<machine_conf>;
}

TEST_CASE("on_event")
{
    using namespace on_event_ns;

    auto machine = machine_t{};
    auto& ctx = machine.context();

    machine.start();

    ctx.out.clear();
    machine.process_event(events::button_press{"a"});
    REQUIRE(machine.is<states::on>());
    REQUIRE(ctx.out == "a1;");

    ctx.out.clear();
    machine.process_event(events::alert_button_press{});
    REQUIRE(machine.is<states::on>());
    REQUIRE(ctx.out == "beep;");
}
