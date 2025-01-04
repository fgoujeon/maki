//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace composite_state_context_ns
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
                using parent_context_type = composite_state_context_ns::context;

                context(parent_context_type& parent):
                    parent(parent)
                {
                }

                parent_context_type& parent;
                int red_count = 0;
            };

            constexpr auto emitting_red = maki::state_conf{}
                .entry_action_c
                (
                    maki::any,
                    [](context& ctx)
                    {
                        ++ctx.red_count;
                    }
                )
            ;

            EMPTY_STATE(emitting_green)
            EMPTY_STATE(emitting_blue)

            constexpr auto transition_table = maki::transition_table{}
                (emitting_red,   maki::type<events::color_button_press>, emitting_green)
                (emitting_green, maki::type<events::color_button_press>, emitting_blue)
                (emitting_blue,  maki::type<events::color_button_press>, emitting_red)
            ;
        }

        constexpr auto on = maki::state_conf{}
            .transition_tables(on_ns::transition_table)
            .context_c<on_ns::context>()
            .exit_action_c
            (
                maki::any,
                [](on_ns::context& ctx)
                {
                    ctx.parent.out = std::to_string(ctx.red_count);
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
        .context_a<context>()
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("composite_state_context")
{
    using namespace composite_state_context_ns;

    auto machine = machine_t{};
    auto& ctx = machine.context();
    const auto on_state = machine.state<states::on>();

    machine.process_event(events::power_button_press{});
    REQUIRE(on_state.is<states::on_ns::emitting_red>());

    machine.process_event(events::color_button_press{});
    REQUIRE(on_state.is<states::on_ns::emitting_green>());

    machine.process_event(events::color_button_press{});
    REQUIRE(on_state.is<states::on_ns::emitting_blue>());

    machine.process_event(events::color_button_press{});
    REQUIRE(on_state.is<states::on_ns::emitting_red>());

    machine.process_event(events::power_button_press{});
    REQUIRE(machine.is<states::off>());

    REQUIRE(ctx.out == "2");

    machine.process_event(events::power_button_press{});
    REQUIRE(machine.is<states::on>());
}
