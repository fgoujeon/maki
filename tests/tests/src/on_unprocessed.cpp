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
        struct off
        {
            static constexpr auto conf = maki::state_conf_c
                .on_event<events::ignored_by_emitting_blue>()
            ;

            void on_event(const events::ignored_by_emitting_blue & /*unused*/)
            {
            }

            context& ctx;
        };

        struct emitting_red
        {
            static constexpr auto conf = maki::state_conf_c
                .enable_on_event_auto()
            ;

            void on_event(const events::ignored_by_emitting_blue & /*unused*/)
            {
            }

            context& ctx;
        };

        struct emitting_green
        {
            static constexpr auto conf = maki::state_conf_c
                .enable_on_event_auto()
            ;

            void on_event(const events::ignored_by_emitting_blue & /*unused*/)
            {
            }

            context& ctx;
        };

        struct emitting_blue
        {
            static constexpr auto conf = maki::state_conf_c;
        };

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
        ::add<states::on, events::power_button_press, states::off>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::machine_conf_c
            .set_transition_tables<transition_table_t>()
            .set_context_type<context>()
            .on_unprocessed()
        ;

        template<class T>
        void on_unprocessed(const T& /*unused*/)
        {
            ctx.ignored_event = "other";
        }

        void on_unprocessed(const events::ignored_by_emitting_blue& event)
        {
            ctx.ignored_event = "ignored_by_emitting_blue{" + std::to_string(event.value) + "}";
        }

        context& ctx;
    };
}

TEST_CASE("on_unprocessed")
{
    auto machine = machine_t{};
    auto& ctx = machine.context();

    ctx.clear();
    machine.start();
    REQUIRE(machine.is_active_state<states::on>());
    REQUIRE(machine.is_active_state<maki::region_path<machine_def>::add<states::on>, states::emitting_red>());
    REQUIRE(ctx.ignored_event.empty());

    ctx.clear();
    machine.process_event(events::ignored_by_emitting_blue{});
    REQUIRE(ctx.ignored_event.empty());

    ctx.clear();
    machine.process_event(events::color_button_press{});
    REQUIRE(machine.is_active_state<states::on>());
    REQUIRE(machine.is_active_state<maki::region_path<machine_def>::add<states::on>, states::emitting_green>());
    REQUIRE(ctx.ignored_event.empty());

    ctx.clear();
    machine.process_event(events::ignored_by_emitting_blue{});
    REQUIRE(ctx.ignored_event.empty());

    ctx.clear();
    machine.process_event(events::color_button_press{});
    REQUIRE(machine.is_active_state<states::on>());
    REQUIRE(machine.is_active_state<maki::region_path<machine_def>::add<states::on>, states::emitting_blue>());
    REQUIRE(ctx.ignored_event.empty());

    ctx.clear();
    machine.process_event(events::ignored_by_emitting_blue{42});
    REQUIRE(ctx.ignored_event == "ignored_by_emitting_blue{42}");

    ctx.clear();
    machine.process_event(events::power_button_press{});
    REQUIRE(machine.is_active_state<states::off>());
    REQUIRE(ctx.ignored_event.empty());
}
