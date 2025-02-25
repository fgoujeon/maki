//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace post_processing_hook_ns
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

        struct ignored_by_all{};

        struct ignored_by_emitting_blue
        {
            int value = 0;
        };
    }

    namespace states
    {
        constexpr auto off = maki::state_conf{}
            .internal_action_v<events::ignored_by_emitting_blue>
            (
                []{}
            )
        ;

        constexpr auto emitting_red = maki::state_conf{}
            .internal_action_v<events::ignored_by_emitting_blue>
            (
                []{}
            )
        ;

        constexpr auto emitting_green = maki::state_conf{}
            .internal_action_v<events::ignored_by_emitting_blue>
            (
                []{}
            )
        ;

        constexpr auto emitting_blue = maki::state_conf{};

        constexpr auto on_transition_table = maki::transition_table{}
            (states::emitting_red,   states::emitting_green, maki::event<events::color_button_press>)
            (states::emitting_green, states::emitting_blue, maki::event<events::color_button_press>)
            (states::emitting_blue,  states::emitting_red, maki::event<events::color_button_press>)
        ;

        constexpr auto on = maki::state_conf{}
            .transition_tables(on_transition_table)
        ;
    }

    constexpr auto transition_table = maki::transition_table{}
        (states::on, states::off, maki::event<events::power_button_press>)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
        .post_processing_hook_mep<events::ignored_by_emitting_blue>
        (
            [](auto& mach, const events::ignored_by_emitting_blue& event, const bool processed)
            {
                if(!processed)
                {
                    mach.context().ignored_event = "ignored_by_emitting_blue{" + std::to_string(event.value) + "}";
                }
            }
        )
        .post_processing_hook_mep
        (
            maki::all_events,
            [](auto& mach, const auto& /*event*/, const bool processed)
            {
                if(!processed)
                {
                    mach.context().ignored_event = "other";
                }
            }
        ).
        auto_start(false)
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("post_processing_hook")
{
    using namespace post_processing_hook_ns;

    auto machine = machine_t{};
    auto& ctx = machine.context();
    const auto& on_state = machine.state<states::on>();

    //Nothing should happen before `start()` is called.
    ctx.clear();
    machine.process_event(events::ignored_by_all{});
    REQUIRE(ctx.ignored_event.empty());

    ctx.clear();
    machine.start();
    REQUIRE(machine.is<states::on>());
    REQUIRE(on_state.is<states::emitting_red>());
    REQUIRE(ctx.ignored_event.empty());

    ctx.clear();
    machine.process_event(events::ignored_by_all{});
    REQUIRE(ctx.ignored_event == "other");

    ctx.clear();
    machine.process_event(events::ignored_by_emitting_blue{});
    REQUIRE(ctx.ignored_event.empty());

    ctx.clear();
    machine.process_event(events::color_button_press{});
    REQUIRE(machine.is<states::on>());
    REQUIRE(on_state.is<states::emitting_green>());
    REQUIRE(ctx.ignored_event.empty());

    ctx.clear();
    machine.process_event(events::ignored_by_emitting_blue{});
    REQUIRE(ctx.ignored_event.empty());

    ctx.clear();
    machine.process_event(events::color_button_press{});
    REQUIRE(machine.is<states::on>());
    REQUIRE(on_state.is<states::emitting_blue>());
    REQUIRE(ctx.ignored_event.empty());

    ctx.clear();
    machine.process_event(events::ignored_by_emitting_blue{42});
    REQUIRE(ctx.ignored_event == "ignored_by_emitting_blue{42}");

    ctx.clear();
    machine.process_event(events::power_button_press{});
    REQUIRE(machine.is<states::off>());
    REQUIRE(ctx.ignored_event.empty());
}
