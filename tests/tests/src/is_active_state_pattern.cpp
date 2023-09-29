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

        using not_emitting_red = maki::any_but<emitting_red>;
        using emitting_red_or_green = maki::any_of<emitting_red, emitting_green>;

        using on_transition_table = maki::transition_table
            ::add<states::emitting_red,   events::color_button_press, states::emitting_green>
            ::add<states::emitting_green, events::color_button_press, states::emitting_blue>
            ::add<states::emitting_blue,  events::color_button_press, states::emitting_red>
        ;

        struct on
        {
            static constexpr auto conf = maki::submachine_conf_c
                .set_transition_tables<on_transition_table>()
            ;

            context& ctx;
        };
    }

    using transition_table_t = maki::transition_table
        ::add<states::off, events::power_button_press, states::on>
        ::add<states::on,  events::power_button_press, states::off>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::machine_conf_c
            .set_transition_tables<transition_table_t>()
            .set_context_type<context>()
        ;
    };
}

TEST_CASE("is_active_state_filter")
{
    using machine_on_region_path_t = maki::region_path<machine_def>::add<states::on>;

    auto machine = machine_t{};

    machine.start();

    REQUIRE(machine.is_active_state<states::off>());
    REQUIRE(!machine.is_running<machine_on_region_path_t>());

    machine.process_event(events::power_button_press{});
    REQUIRE(!machine.is_active_state<states::emitting_red_or_green>());
    REQUIRE(machine.is_active_state<machine_on_region_path_t, states::emitting_red>());
    REQUIRE(machine.is_active_state<machine_on_region_path_t, states::emitting_red_or_green>());
    REQUIRE(!machine.is_active_state<machine_on_region_path_t, states::not_emitting_red>());

    machine.process_event(events::color_button_press{});
    REQUIRE(machine.is_active_state<machine_on_region_path_t, states::emitting_green>());
    REQUIRE(machine.is_active_state<machine_on_region_path_t, states::emitting_red_or_green>());
    REQUIRE(machine.is_active_state<machine_on_region_path_t, states::not_emitting_red>());

    machine.process_event(events::color_button_press{});
    REQUIRE(machine.is_active_state<machine_on_region_path_t, states::emitting_blue>());
    REQUIRE(!machine.is_active_state<machine_on_region_path_t, states::emitting_red_or_green>());
    REQUIRE(machine.is_active_state<machine_on_region_path_t, states::not_emitting_red>());

    machine.process_event(events::power_button_press{});
    REQUIRE(machine.is_active_state<states::off>());
    REQUIRE(!machine.is_active_state<states::emitting_red_or_green>());
    REQUIRE(machine.is_active_state<machine_on_region_path_t, maki::states::stopped>());
    REQUIRE(!machine.is_active_state<machine_on_region_path_t, states::emitting_red_or_green>());
    REQUIRE(machine.is_active_state<machine_on_region_path_t, states::not_emitting_red>());

    machine.process_event(events::power_button_press{});
    REQUIRE(machine.is_active_state<states::on>());
}
