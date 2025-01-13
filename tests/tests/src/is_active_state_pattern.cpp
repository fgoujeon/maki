//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace is_active_state_pattern_ns
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

        constexpr auto not_emitting_red = maki::any_state_but(emitting_red);
        constexpr auto emitting_red_or_green = maki::any_state_of(emitting_red, emitting_green);

        constexpr auto on_transition_table = maki::transition_table{}
            (states::emitting_red,   maki::event<events::color_button_press>, states::emitting_green)
            (states::emitting_green, maki::event<events::color_button_press>, states::emitting_blue)
            (states::emitting_blue,  maki::event<events::color_button_press>, states::emitting_red)
        ;

        constexpr auto on = maki::state_conf{}
            .transition_tables(on_transition_table)
        ;
    }

    constexpr auto transition_table = maki::transition_table{}
        (states::off, maki::event<events::power_button_press>, states::on)
        (states::on,  maki::event<events::power_button_press>, states::off)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("is_active_state_filter")
{
    using namespace is_active_state_pattern_ns;

    auto machine = machine_t{};
    const auto& on_state = machine.state<states::on>();

    machine.start();

    REQUIRE(machine.is<states::off>());
    REQUIRE(!on_state.running());

    machine.process_event(events::power_button_press{});
    REQUIRE(!machine.is<states::emitting_red_or_green>());
    REQUIRE(on_state.is<states::emitting_red>());
    REQUIRE(on_state.is<states::emitting_red_or_green>());
    REQUIRE(!on_state.is<states::not_emitting_red>());

    machine.process_event(events::color_button_press{});
    REQUIRE(on_state.is<states::emitting_green>());
    REQUIRE(on_state.is<states::emitting_red_or_green>());
    REQUIRE(on_state.is<states::not_emitting_red>());

    machine.process_event(events::color_button_press{});
    REQUIRE(on_state.is<states::emitting_blue>());
    REQUIRE(!on_state.is<states::emitting_red_or_green>());
    REQUIRE(on_state.is<states::not_emitting_red>());

    machine.process_event(events::power_button_press{});
    REQUIRE(machine.is<states::off>());
    REQUIRE(!machine.is<states::emitting_red_or_green>());
    REQUIRE(on_state.is<maki::state_confs::stopped>());
    REQUIRE(!on_state.is<states::emitting_red_or_green>());
    REQUIRE(on_state.is<states::not_emitting_red>());

    machine.process_event(events::power_button_press{});
    REQUIRE(machine.is<states::on>());
}
