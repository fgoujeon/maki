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
        EMPTY_STATE(off);

        struct emitting_red
        {
            static constexpr auto conf = maki::state_conf_c
                .enable_on_entry()
            ;

            void on_entry()
            {
                ctx.current_led_color = led_color::red;
            }

            context& ctx;
            led_color color = led_color::red;
        };

        struct emitting_green
        {
            static constexpr auto conf = maki::state_conf_c
                .enable_on_entry()
            ;

            void on_entry()
            {
                ctx.current_led_color = led_color::green;
            }

            context& ctx;
            led_color color = led_color::green;
        };

        struct emitting_blue
        {
            static constexpr auto conf = maki::state_conf_c
                .enable_on_entry()
            ;

            void on_entry()
            {
                ctx.current_led_color = led_color::blue;
            }

            context& ctx;
            led_color color = led_color::blue;
        };

        constexpr auto on_transition_table = maki::transition_table_c
            .add<states::emitting_red,   events::color_button_press, states::emitting_green>
            .add<states::emitting_green, events::color_button_press, states::emitting_blue>
            .add<states::emitting_blue,  events::color_button_press, states::emitting_red>
        ;

        struct on
        {
            static constexpr auto conf = maki::submachine_conf_c
                .set_transition_tables(on_transition_table)
                .enable_on_exit()
            ;

            void on_exit()
            {
                ctx.current_led_color = led_color::off;
            }

            context& ctx;
            bool is_on_state = true;
        };
    }

    constexpr auto transition_table = maki::transition_table_c
        .add<states::off, events::power_button_press, states::on>
        .add<states::on,  events::power_button_press, states::off>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::machine_conf_c
            .set_transition_tables(transition_table)
            .set_context_type<context>()
        ;
    };

    using machine_t = maki::machine<machine_def>;
}

TEST_CASE("state")
{
    auto machine = machine_t{};
    const auto& const_sm = machine;

    using root_region_path = maki::region_path<machine_def>;
    using on_region_path = root_region_path::add<states::on>;

    auto& red_state = machine.state<on_region_path, states::emitting_red>();
    REQUIRE(red_state.color == led_color::red);

    const auto& green_state = const_sm.state<on_region_path, states::emitting_green>();
    REQUIRE(green_state.color == led_color::green);

    auto& blue_state = machine.state<on_region_path, states::emitting_blue>();
    REQUIRE(blue_state.color == led_color::blue);

    auto& on_state = machine.state<root_region_path, states::on>();
    REQUIRE(on_state.is_on_state);
}
