//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace
{
    struct machine_conf_holder;
    using machine_t = maki::machine<machine_conf_holder>;

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
        EMPTY_STATE(off)
        EMPTY_STATE(emitting_red)
        EMPTY_STATE(emitting_green)
        EMPTY_STATE(emitting_blue)

        constexpr auto not_emitting_red = maki::any_but_c<emitting_red>;
        constexpr auto emitting_red_or_green = maki::any_of_c<emitting_red, emitting_green>;

        constexpr auto on_transition_table = maki::transition_table{}
            .add_c<states::emitting_red,   events::color_button_press, states::emitting_green>
            .add_c<states::emitting_green, events::color_button_press, states::emitting_blue>
            .add_c<states::emitting_blue,  events::color_button_press, states::emitting_red>
        ;

        constexpr auto on = maki::submachine_conf{}
            .transition_tables(on_transition_table)
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

    struct machine_conf_holder: maki::conf_holder<machine_conf>{};
}

TEST_CASE("is_active_state_filter")
{
    static constexpr auto on_reg_path = maki::path{0} / states::on / 0;

    auto machine = machine_t{};

    machine.start();

    REQUIRE(machine.active_state<states::off>());
    REQUIRE(!machine.running<on_reg_path>());

    machine.process_event(events::power_button_press{});
    REQUIRE(!machine.active_state<states::emitting_red_or_green>());
    REQUIRE(machine.active_state<on_reg_path, states::emitting_red>());
    REQUIRE(machine.active_state<on_reg_path, states::emitting_red_or_green>());
    REQUIRE(!machine.active_state<on_reg_path, states::not_emitting_red>());

    machine.process_event(events::color_button_press{});
    REQUIRE(machine.active_state<on_reg_path, states::emitting_green>());
    REQUIRE(machine.active_state<on_reg_path, states::emitting_red_or_green>());
    REQUIRE(machine.active_state<on_reg_path, states::not_emitting_red>());

    machine.process_event(events::color_button_press{});
    REQUIRE(machine.active_state<on_reg_path, states::emitting_blue>());
    REQUIRE(!machine.active_state<on_reg_path, states::emitting_red_or_green>());
    REQUIRE(machine.active_state<on_reg_path, states::not_emitting_red>());

    machine.process_event(events::power_button_press{});
    REQUIRE(machine.active_state<states::off>());
    REQUIRE(!machine.active_state<states::emitting_red_or_green>());
    REQUIRE(machine.active_state<on_reg_path, maki::state_confs::stopped>());
    REQUIRE(!machine.active_state<on_reg_path, states::emitting_red_or_green>());
    REQUIRE(machine.active_state<on_reg_path, states::not_emitting_red>());

    machine.process_event(events::power_button_press{});
    REQUIRE(machine.active_state<states::on>());
}
