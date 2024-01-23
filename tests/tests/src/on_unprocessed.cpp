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
        constexpr auto off = maki::state_conf{}
            .internal_action_v<maki::type_c<events::ignored_by_emitting_blue>>([]{})
        ;

        constexpr auto emitting_red = maki::state_conf{}
            .internal_action_v<maki::type_c<events::ignored_by_emitting_blue>>([]{})
        ;

        constexpr auto emitting_green = maki::state_conf{}
            .internal_action_v<maki::type_c<events::ignored_by_emitting_blue>>([]{})
        ;

        constexpr auto emitting_blue = maki::state_conf{};

        constexpr auto on_transition_table = maki::transition_table{}
            (states::emitting_red,   maki::type_c<events::color_button_press>, states::emitting_green)
            (states::emitting_green, maki::type_c<events::color_button_press>, states::emitting_blue)
            (states::emitting_blue,  maki::type_c<events::color_button_press>, states::emitting_red)
        ;

        constexpr auto on = maki::submachine_conf{}
            .transition_tables(on_transition_table)
        ;
    }

    constexpr auto transition_table = maki::transition_table{}
        (states::on, maki::type_c<events::power_button_press>, states::off)
    ;

    struct machine_conf_holder
    {
        static constexpr auto conf = maki::machine_conf{}
            .transition_tables(transition_table)
            .context<context>()
            .fallback_transition_action_me<maki::type_c<events::ignored_by_emitting_blue>>
            (
                [](auto& mach, const events::ignored_by_emitting_blue& event)
                {
                    mach.context().ignored_event = "ignored_by_emitting_blue{" + std::to_string(event.value) + "}";
                }
            )
            .fallback_transition_action_me<maki::any>
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

    static constexpr auto on_path = maki::path{0} / states::on / 0;

    ctx.clear();
    machine.start();
    REQUIRE(machine.active_state<states::on>());
    REQUIRE(machine.active_state<on_path, states::emitting_red>());
    REQUIRE(ctx.ignored_event.empty());

    ctx.clear();
    machine.process_event(events::ignored_by_emitting_blue{});
    REQUIRE(ctx.ignored_event.empty());

    ctx.clear();
    machine.process_event(events::color_button_press{});
    REQUIRE(machine.active_state<states::on>());
    REQUIRE(machine.active_state<on_path, states::emitting_green>());
    REQUIRE(ctx.ignored_event.empty());

    ctx.clear();
    machine.process_event(events::ignored_by_emitting_blue{});
    REQUIRE(ctx.ignored_event.empty());

    ctx.clear();
    machine.process_event(events::color_button_press{});
    REQUIRE(machine.active_state<states::on>());
    REQUIRE(machine.active_state<on_path, states::emitting_blue>());
    REQUIRE(ctx.ignored_event.empty());

    ctx.clear();
    machine.process_event(events::ignored_by_emitting_blue{42});
    REQUIRE(ctx.ignored_event == "ignored_by_emitting_blue{42}");

    ctx.clear();
    machine.process_event(events::power_button_press{});
    REQUIRE(machine.active_state<states::off>());
    REQUIRE(ctx.ignored_event.empty());
}
