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

    enum class led_color
    {
        off,
        red,
        green,
        blue
    };

    struct context
    {
        led_color current_led_color = led_color::off;
    };

    namespace events
    {
        struct power_button_press{};
        struct color_button_press{};
    }

    namespace states
    {
        EMPTY_STATE(off)

        struct emitting_red_data
        {
            machine_t& machine;
            context& ctx;
        };

        constexpr auto emitting_red = maki::state_conf
            .data<emitting_red_data>()
            .entry_action_c<maki::any_t>
            (
                [](context& ctx)
                {
                    ctx.current_led_color = led_color::red;
                }
            )
        ;

        struct emitting_green_data
        {
            machine_t& machine;
            context& ctx;
        };

        constexpr auto emitting_green = maki::state_conf
            .data<emitting_green_data>()
            .entry_action_c<maki::any_t>
            (
                [](context& ctx)
                {
                    ctx.current_led_color = led_color::green;
                }
            )
        ;

        struct emitting_blue_data
        {
            machine_t& machine;
            context& ctx;
        };

        constexpr auto emitting_blue = maki::state_conf
            .data<emitting_blue_data>()
            .entry_action_c<maki::any_t>
            (
                [](context& ctx)
                {
                    ctx.current_led_color = led_color::blue;
                }
            )
        ;

        constexpr auto on_transition_table = maki::empty_transition_table
            .add_c<states::emitting_red,   events::color_button_press, states::emitting_green>
            .add_c<states::emitting_green, events::color_button_press, states::emitting_blue>
            .add_c<states::emitting_blue,  events::color_button_press, states::emitting_red>
        ;

        constexpr auto on = maki::submachine_conf
            .transition_tables(on_transition_table)
            .exit_action_c<maki::any_t>
            (
                [](context& ctx)
                {
                    ctx.current_led_color = led_color::off;
                }
            )
        ;
    }

    constexpr auto transition_table = maki::empty_transition_table
        .add_c<states::off, events::power_button_press, states::on>
        .add_c<states::on,  events::power_button_press, states::off>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::default_machine_conf
            .transition_tables(transition_table)
            .context<context>()
        ;
    };
}

TEST_CASE("submachine")
{
    static constexpr auto machine_on_region_path = maki::region_path_c<machine_def>.add<states::on>();

    auto machine = machine_t{};
    auto& ctx = machine.context();

    machine.start();

    REQUIRE(machine.is_active_state<states::off>());
    REQUIRE(!machine.is_running<machine_on_region_path>());
    REQUIRE(ctx.current_led_color == led_color::off);

    machine.process_event(events::power_button_press{});
    REQUIRE(machine.is_active_state<states::on>());
    REQUIRE(machine.is_active_state<machine_on_region_path, states::emitting_red>());
    REQUIRE(ctx.current_led_color == led_color::red);

    machine.process_event(events::color_button_press{});
    REQUIRE(machine.is_active_state<states::on>());
    REQUIRE(ctx.current_led_color == led_color::green);

    machine.process_event(events::color_button_press{});
    REQUIRE(machine.is_active_state<states::on>());
    REQUIRE(ctx.current_led_color == led_color::blue);

    machine.process_event(events::power_button_press{});
    REQUIRE(machine.is_active_state<states::off>());
    REQUIRE(ctx.current_led_color == led_color::off);

    machine.process_event(events::power_button_press{});
    REQUIRE(machine.is_active_state<states::on>());
    REQUIRE(ctx.current_led_color == led_color::red);
}
