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
        void clear()
        {
            ignored_event = "";
        }

        std::string ignored_event;
    };

    namespace events
    {
        struct power_button_press{};
        struct color_button_press{};

        struct ignored_by_emitting_blue
        {
            int value = 0;
        };
    }

    namespace states
    {
        constexpr auto off = maki::state_conf
            .internal_action_v<events::ignored_by_emitting_blue>([]{})
        ;

        constexpr auto emitting_red = maki::state_conf
            .internal_action_v<events::ignored_by_emitting_blue>([]{})
        ;

        constexpr auto emitting_green = maki::state_conf
            .internal_action_v<events::ignored_by_emitting_blue>([]{})
        ;

        constexpr auto emitting_blue = maki::state_conf;

        constexpr auto on_transition_table = maki::empty_transition_table
            .add_c<states::emitting_red,   events::color_button_press, states::emitting_green>
            .add_c<states::emitting_green, events::color_button_press, states::emitting_blue>
            .add_c<states::emitting_blue,  events::color_button_press, states::emitting_red>
        ;

        constexpr auto on = maki::submachine_conf
            .set_transition_tables(on_transition_table)
        ;
    }

    constexpr auto transition_table = maki::empty_transition_table
        .add_c<states::on, events::power_button_press, states::off>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::default_machine_conf
            .set_transition_tables(transition_table)
            .set_context<context>()
            .unprocessed_action_me<events::ignored_by_emitting_blue>
            (
                [](auto& mach, const events::ignored_by_emitting_blue& event)
                {
                    mach.context().ignored_event = "ignored_by_emitting_blue{" + std::to_string(event.value) + "}";
                }
            )
            .unprocessed_action_me<maki::any>
            (
                [](auto& mach, const auto& /*event*/)
                {
                    mach.context().ignored_event = "other";
                }
            )
        ;
    };
}

TEST_CASE("on_unprocessed")
{
    auto machine = machine_t{};
    auto& ctx = machine.context();

    static constexpr auto on_region_path = maki::region_path_c<machine_def>.add<states::on>();

    ctx.clear();
    machine.start();
    REQUIRE(machine.is_active_state<states::on>());
    REQUIRE(machine.is_active_state<on_region_path, states::emitting_red>());
    REQUIRE(ctx.ignored_event.empty());

    ctx.clear();
    machine.process_event(events::ignored_by_emitting_blue{});
    REQUIRE(ctx.ignored_event.empty());

    ctx.clear();
    machine.process_event(events::color_button_press{});
    REQUIRE(machine.is_active_state<states::on>());
    REQUIRE(machine.is_active_state<on_region_path, states::emitting_green>());
    REQUIRE(ctx.ignored_event.empty());

    ctx.clear();
    machine.process_event(events::ignored_by_emitting_blue{});
    REQUIRE(ctx.ignored_event.empty());

    ctx.clear();
    machine.process_event(events::color_button_press{});
    REQUIRE(machine.is_active_state<states::on>());
    REQUIRE(machine.is_active_state<on_region_path, states::emitting_blue>());
    REQUIRE(ctx.ignored_event.empty());

    ctx.clear();
    machine.process_event(events::ignored_by_emitting_blue{42});
    REQUIRE(ctx.ignored_event == "ignored_by_emitting_blue{42}");

    ctx.clear();
    machine.process_event(events::power_button_press{});
    REQUIRE(machine.is_active_state<states::off>());
    REQUIRE(ctx.ignored_event.empty());
}
