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
    };

    namespace events
    {
        struct power_button_press{};
        struct color_button_press{};
    }

    namespace states
    {
        EMPTY_STATE(off);
        EMPTY_STATE(emitting_red);
        EMPTY_STATE(emitting_green);
        EMPTY_STATE(emitting_blue);

        constexpr auto not_emitting_red = maki::any_but_c<emitting_red>;
        constexpr auto emitting_red_or_green = maki::any_of_c<emitting_red, emitting_green>;

        constexpr auto on_transition_table = maki::empty_transition_table
            .add_c<states::emitting_red,   events::color_button_press, states::emitting_green>
            .add_c<states::emitting_green, events::color_button_press, states::emitting_blue>
            .add_c<states::emitting_blue,  events::color_button_press, states::emitting_red>
        ;

        constexpr auto on = maki::submachine_conf_c<>
            .set_transition_tables(on_transition_table)
        ;
    }

    constexpr auto transition_table = maki::empty_transition_table
        .add_c<states::off, events::power_button_press, states::on>
        .add_c<states::on,  events::power_button_press, states::off>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::default_machine_conf
            .set_transition_tables(transition_table)
            .set_context<context>()
        ;
    };
}

TEST_CASE("is_active_state_filter")
{
    static constexpr auto machine_on_region_path_v = maki::region_path_c<machine_def>.add<states::on>();

    auto machine = machine_t{};

    machine.start();

    REQUIRE(machine.is_active_state<states::off>());
    REQUIRE(!machine.is_running<machine_on_region_path_v>());

    machine.process_event(events::power_button_press{});
    REQUIRE(!machine.is_active_state<states::emitting_red_or_green>());
    REQUIRE(machine.is_active_state<machine_on_region_path_v, states::emitting_red>());
    REQUIRE(machine.is_active_state<machine_on_region_path_v, states::emitting_red_or_green>());
    REQUIRE(!machine.is_active_state<machine_on_region_path_v, states::not_emitting_red>());

    machine.process_event(events::color_button_press{});
    REQUIRE(machine.is_active_state<machine_on_region_path_v, states::emitting_green>());
    REQUIRE(machine.is_active_state<machine_on_region_path_v, states::emitting_red_or_green>());
    REQUIRE(machine.is_active_state<machine_on_region_path_v, states::not_emitting_red>());

    machine.process_event(events::color_button_press{});
    REQUIRE(machine.is_active_state<machine_on_region_path_v, states::emitting_blue>());
    REQUIRE(!machine.is_active_state<machine_on_region_path_v, states::emitting_red_or_green>());
    REQUIRE(machine.is_active_state<machine_on_region_path_v, states::not_emitting_red>());

    machine.process_event(events::power_button_press{});
    REQUIRE(machine.is_active_state<states::off>());
    REQUIRE(!machine.is_active_state<states::emitting_red_or_green>());
    REQUIRE(machine.is_active_state<machine_on_region_path_v, maki::states::stopped>());
    REQUIRE(!machine.is_active_state<machine_on_region_path_v, states::emitting_red_or_green>());
    REQUIRE(machine.is_active_state<machine_on_region_path_v, states::not_emitting_red>());

    machine.process_event(events::power_button_press{});
    REQUIRE(machine.is_active_state<states::on>());
}
