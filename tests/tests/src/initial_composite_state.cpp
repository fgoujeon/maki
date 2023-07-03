//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include "common.hpp"

namespace
{
    struct machine_def;
    using machine_t = awesm::machine<machine_def>;

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
        struct off
        {
            using conf = awesm::state_conf
                ::on_entry_any
            ;

            void on_entry()
            {
                ctx.current_led_color = led_color::off;
            }

            context& ctx;
        };

        struct emitting_red
        {
            using conf = awesm::state_conf
                ::on_entry_any
            ;

            void on_entry()
            {
                ctx.current_led_color = led_color::red;
            }

            context& ctx;
        };

        struct emitting_green
        {
            using conf = awesm::state_conf
                ::on_entry_any
            ;

            void on_entry()
            {
                ctx.current_led_color = led_color::green;
            }

            context& ctx;
        };

        struct emitting_blue
        {
            using conf = awesm::state_conf
                ::on_entry_any
            ;

            void on_entry()
            {
                ctx.current_led_color = led_color::blue;
            }

            context& ctx;
        };

        using on_transition_table = awesm::transition_table
            ::add<states::emitting_red,   events::color_button_press, states::emitting_green>
            ::add<states::emitting_green, events::color_button_press, states::emitting_blue>
            ::add<states::emitting_blue,  events::color_button_press, states::emitting_red>
        ;

        struct on
        {
            using conf = awesm::submachine_conf
                ::transition_tables<on_transition_table>
            ;

            context& ctx;
        };
    }

    using transition_table_t = awesm::transition_table
        ::add<states::on, events::power_button_press, states::off>
    ;

    struct machine_def
    {
        using conf = awesm::machine_conf
            ::transition_tables<transition_table_t>
            ::context<context>
        ;
    };
}

TEST_CASE("initial_submachine")
{
    auto machine = machine_t{};
    auto& ctx = machine.context();

    machine.start();
    REQUIRE(machine.is_active_state<states::on>());
    REQUIRE(machine.is_active_state<awesm::region_path<machine_def>::add<states::on>, states::emitting_red>());
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
}
