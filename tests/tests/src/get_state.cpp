//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"
#include <string>

namespace get_state_ns
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
        EMPTY_STATE(off)

        struct on_data
        {
            on_data(context& parent):
                parent(parent)
            {
            }

            context& parent;
            bool is_on_state = true;
        };

        struct emitting_red_data
        {
            emitting_red_data(on_data& parent):
                parent(parent)
            {
            }

            on_data& parent;
            led_color color = led_color::red;
        };

        constexpr auto emitting_red = maki::state_conf{}
            .context_c(maki::type<emitting_red_data>)
            .entry_action_c
            (
                maki::any,
                [](emitting_red_data& ctx)
                {
                    ctx.parent.parent.current_led_color = led_color::red;
                }
            )
        ;

        struct emitting_green_data
        {
            emitting_green_data(on_data& parent):
                parent(parent)
            {
            }

            on_data& parent;
            led_color color = led_color::green;
        };

        constexpr auto emitting_green = maki::state_conf{}
            .context_c(maki::type<emitting_green_data>)
            .entry_action_c
            (
                maki::any,
                [](emitting_green_data& ctx)
                {
                    ctx.parent.parent.current_led_color = led_color::green;
                }
            )
        ;

        struct emitting_blue_data
        {
            emitting_blue_data(on_data& parent):
                parent(parent)
            {
            }

            on_data& parent;
            led_color color = led_color::blue;
        };

        constexpr auto emitting_blue = maki::state_conf{}
            .context_c(maki::type<emitting_blue_data>)
            .entry_action_c
            (
                maki::any,
                [](emitting_blue_data& ctx)
                {
                    ctx.parent.parent.current_led_color = led_color::blue;
                }
            )
        ;

        constexpr auto on_transition_table = maki::transition_table{}
            (states::emitting_red,   maki::type<events::color_button_press>, states::emitting_green)
            (states::emitting_green, maki::type<events::color_button_press>, states::emitting_blue)
            (states::emitting_blue,  maki::type<events::color_button_press>, states::emitting_red)
        ;

        constexpr auto on = maki::state_conf{}
            .context_c(maki::type<on_data>)
            .transition_tables(on_transition_table)
            .exit_action_c
            (
                maki::any,
                [](context& ctx)
                {
                    ctx.current_led_color = led_color::off;
                }
            )
        ;
    }

    constexpr auto transition_table = maki::transition_table{}
        (states::off, maki::type<events::power_button_press>, states::on)
        (states::on,  maki::type<events::power_button_press>, states::off)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a(maki::type<context>)
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("state")
{
    using namespace get_state_ns;

    auto machine = machine_t{};
    const auto on_state = machine.state<states::on>();
    const auto emitting_red_state = on_state.state<states::emitting_red>();
    const auto emitting_green_state = on_state.state<states::emitting_green>();
    const auto emitting_blue_state = on_state.state<states::emitting_blue>();

    REQUIRE(emitting_red_state.context().color == led_color::red);
    REQUIRE(emitting_green_state.context().color == led_color::green);
    REQUIRE(emitting_blue_state.context().color == led_color::blue);
    REQUIRE(on_state.context().is_on_state);
}
