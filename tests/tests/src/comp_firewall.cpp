//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "comp_firewall_private.hpp"
#include "comp_firewall_common.hpp"
#include "common.hpp"
#include <string>

namespace comp_firewall_ns
{
    namespace states
    {
        constexpr auto off = maki::state_mold{}
            .entry_action_c(
                [](context& ctx)
                {
                    ctx.current_led_color = led_color::off;
                })
        ;

        constexpr auto on = maki::forwarder_state_mold{}
            .forwarder<on_forwarder>()
        ;
    }

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini,   states::off)
        (states::off, states::on,  maki::event<events::power_button_press>)
        (states::on,  states::off, maki::event<events::power_button_press>)
    ;

    struct machine_conf
    {
        static constexpr auto value = maki::machine_conf{}
            .transition_tables(transition_table)
            .context_a<context>()
        ;
    };

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("comp_firewall")
{
    using namespace comp_firewall_ns;

    auto machine = machine_t{};
    auto& ctx = machine.context();

    machine.process_event(events::power_button_press{});
    REQUIRE(ctx.current_led_color == led_color::red);

    machine.process_event(events::color_button_press{});
    REQUIRE(ctx.current_led_color == led_color::green);

    machine.process_event(events::color_button_press{});
    REQUIRE(ctx.current_led_color == led_color::blue);

    machine.process_event(events::color_button_press{});
    REQUIRE(ctx.current_led_color == led_color::red);

    machine.process_event(events::power_button_press{});
    REQUIRE(ctx.current_led_color == led_color::off);

    machine.process_event(events::power_button_press{});
    REQUIRE(ctx.current_led_color == led_color::red);
}
