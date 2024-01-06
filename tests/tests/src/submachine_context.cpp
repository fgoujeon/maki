//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

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
        std::string out;
    };

    namespace events
    {
        struct power_button_press{};
        struct color_button_press{};
    }

    namespace states
    {
        EMPTY_STATE(off)

        namespace on_ns
        {
            struct context
            {
                using parent_context_type = ::context;

                context(parent_context_type& parent):
                    parent(parent)
                {
                }

                parent_context_type& parent;
                int red_count = 0;
            };

            namespace emitting_red_ns
            {
                constexpr auto emitting_cold_red = maki::state_conf{};
                EMPTY_STATE(emitting_hot_red)

                constexpr auto transition_table = maki::transition_table{}
                    .add_c<emitting_cold_red, events::color_button_press, emitting_hot_red>
                ;
            }

            constexpr auto emitting_red = maki::submachine_conf{}
                .transition_tables(emitting_red_ns::transition_table)
                .entry_action_c<maki::any>
                (
                    [](context& ctx)
                    {
                        ++ctx.red_count;
                    }
                )
            ;

            EMPTY_STATE(emitting_green)
            EMPTY_STATE(emitting_blue)

            constexpr auto transition_table = maki::transition_table{}
                .add_c<emitting_red,   events::color_button_press, emitting_green>
                .add_c<emitting_green, events::color_button_press, emitting_blue>
                .add_c<emitting_blue,  events::color_button_press, emitting_red>
            ;
        }

        constexpr auto on = maki::submachine_conf{}
            .transition_tables(on_ns::transition_table)
            .context<on_ns::context>()
            .exit_action_c<maki::any>
            (
                [](on_ns::context& ctx)
                {
                    ctx.parent.out = std::to_string(ctx.red_count);
                }
            )
        ;
    }

    constexpr auto transition_table = maki::transition_table{}
        .add_c<states::off, events::power_button_press, states::on>
        .add_c<states::on,  events::power_button_press, states::off>
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context<context>()
    ;

    using machine_t = maki::make_machine<machine_conf>;
}

TEST_CASE("submachine_context")
{
    static constexpr auto machine_on_path = maki::path{0} / states::on / 0;

    auto machine = machine_t{};
    auto& ctx = machine.context();

    machine.process_event(events::power_button_press{});
    REQUIRE(machine.active_state<machine_on_path, states::on_ns::emitting_red>());

    machine.process_event(events::color_button_press{});
    REQUIRE(machine.active_state<machine_on_path, states::on_ns::emitting_green>());

    machine.process_event(events::color_button_press{});
    REQUIRE(machine.active_state<machine_on_path, states::on_ns::emitting_blue>());

    machine.process_event(events::color_button_press{});
    REQUIRE(machine.active_state<machine_on_path, states::on_ns::emitting_red>());

    machine.process_event(events::power_button_press{});
    REQUIRE(machine.active_state<states::off>());

    REQUIRE(ctx.out == "2");

    machine.process_event(events::power_button_press{});
    REQUIRE(machine.active_state<states::on>());
}
