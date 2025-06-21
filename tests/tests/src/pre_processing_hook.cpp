//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"
#include <string>

namespace pre_processing_hook_ns
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
        constexpr auto off = maki::state_mold{};

        constexpr auto on = maki::state_mold{}
            .internal_action_c<events::button_press>
            (
                [](context& ctx)
                {
                    ctx.out += "_";
                }
            )
            .internal_action_c<events::alert_button_press>
            (
                [](context& ctx)
                {
                    ctx.out += "beep;";
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
        .pre_processing_hook_ce<events::button_press>
        (
            [](context& ctx, const events::button_press& event)
            {
                ctx.out += event.data + "1;";
            }
        )
        .auto_start(false)
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("pre_processing_hook")
{
    using namespace pre_processing_hook_ns;

    auto machine = machine_t{};
    auto& ctx = machine.context();

    //Nothing should happen before `start()` is called.
    ctx.out.clear();
    machine.process_event(events::button_press{"a"});
    REQUIRE(!machine.running());
    REQUIRE(ctx.out == "");

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
