//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace AOS(composite_state_context_ns)
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
                using parent_context_type = AOS(composite_state_context_ns)::context;

                context(parent_context_type& parent):
                    parent(parent)
                {
                }

                parent_context_type& parent;
                int red_count = 0;
            };

            constexpr auto emitting_red = maki::state_mold{}
                .entry_action_c
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
                (maki::ini,      emitting_red)
                (emitting_red,   emitting_green, maki::event<events::color_button_press>)
                (emitting_green, emitting_blue, maki::event<events::color_button_press>)
                (emitting_blue,  emitting_red, maki::event<events::color_button_press>)
            ;
        }

        constexpr auto on = maki::state_mold{}
            .transition_tables(on_ns::transition_table)
            .context_c<on_ns::context>()
            .exit_action_c
            (
                [](on_ns::context& ctx)
                {
                    ctx.parent.out = std::to_string(ctx.red_count);
                }
            )
        ;
    }

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini,   states::off)
        (states::off, states::on, maki::event<events::power_button_press>)
        (states::on,  states::off, maki::event<events::power_button_press>)
    ;

    struct machine_conf
    {
        static constexpr auto value = maki::machine_conf{}
            .transition_tables(transition_table)
            .context_a<context>()
            AOS_ASYNC_OPTS
        ;
    };

    using machine_t = maki::AOS(machine)<machine_conf>;

    AOS_TASK_TYPE(void) test()
    {
        auto machine = machine_t{};
        auto& ctx = machine.context();
        const auto& on_state = machine.state<states::on>();

#if AOS_ASYNC
        co_await machine.start();
#endif

        AOS_CALL machine.process_event(events::power_button_press{});
        REQUIRE(on_state.is<states::on_ns::emitting_red>());

        AOS_CALL machine.process_event(events::color_button_press{});
        REQUIRE(on_state.is<states::on_ns::emitting_green>());

        AOS_CALL machine.process_event(events::color_button_press{});
        REQUIRE(on_state.is<states::on_ns::emitting_blue>());

        AOS_CALL machine.process_event(events::color_button_press{});
        REQUIRE(on_state.is<states::on_ns::emitting_red>());

        AOS_CALL machine.process_event(events::power_button_press{});
        REQUIRE(machine.is<states::off>());

        REQUIRE(ctx.out == "2");

        AOS_CALL machine.process_event(events::power_button_press{});
        REQUIRE(machine.is<states::on>());
    }
}

AOS_TEST_CASE(composite_state_context)
