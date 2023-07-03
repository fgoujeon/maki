//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include "common.hpp"

namespace
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
        EMPTY_STATE(off);

        namespace on_ns
        {
            struct context
            {
                using parent_context_type = ::context;

                context(parent_context_type& parent):
                    parent(parent)
                {
                }

                parent_context_type& parent;
                int red_count = 0;
            };

            namespace emitting_red_ns
            {
                struct emitting_cold_red
                {
                    using conf = awesm::state_conf;
                    on_ns::context& ctx;
                };

                EMPTY_STATE(emitting_hot_red);

                using transition_table_t = awesm::transition_table
                    ::add<emitting_cold_red, events::color_button_press, emitting_hot_red>
                ;
            }

            struct emitting_red
            {
                using conf = awesm::submachine_conf
                    ::transition_tables<emitting_red_ns::transition_table_t>
                    ::on_entry_any
                ;

                void on_entry()
                {
                    ++ctx.red_count;
                }

                context& ctx;
            };

            EMPTY_STATE(emitting_green);
            EMPTY_STATE(emitting_blue);

            using transition_table_t = awesm::transition_table
                ::add<emitting_red,   events::color_button_press, emitting_green>
                ::add<emitting_green, events::color_button_press, emitting_blue>
                ::add<emitting_blue,  events::color_button_press, emitting_red>
            ;
        }

        struct on
        {
            using conf = awesm::submachine_conf
                ::transition_tables<on_ns::transition_table_t>
                ::context<on_ns::context>
                ::on_exit_any
            ;

            void on_exit()
            {
                ctx.parent.out = std::to_string(ctx.red_count);
            }

            on_ns::context& ctx;
        };
    }

    using transition_table_t = awesm::transition_table
        ::add<states::off, events::power_button_press, states::on>
        ::add<states::on,  events::power_button_press, states::off>
    ;

    struct machine_def
    {
        using conf = awesm::machine_conf
            ::transition_tables<transition_table_t>
            ::context<context>
        ;
    };

    using machine_t = awesm::machine<machine_def>;
}

TEST_CASE("submachine_context")
{
    using machine_on_region_path_t = awesm::region_path<machine_def>::add<states::on>;

    auto machine = machine_t{};
    auto& ctx = machine.context();

    machine.process_event(events::power_button_press{});
    REQUIRE(machine.is_active_state<machine_on_region_path_t, states::on_ns::emitting_red>());

    machine.process_event(events::color_button_press{});
    REQUIRE(machine.is_active_state<machine_on_region_path_t, states::on_ns::emitting_green>());

    machine.process_event(events::color_button_press{});
    REQUIRE(machine.is_active_state<machine_on_region_path_t, states::on_ns::emitting_blue>());

    machine.process_event(events::color_button_press{});
    REQUIRE(machine.is_active_state<machine_on_region_path_t, states::on_ns::emitting_red>());

    machine.process_event(events::power_button_press{});
    REQUIRE(machine.is_active_state<states::off>());

    REQUIRE(ctx.out == "2");

    machine.process_event(events::power_button_press{});
    REQUIRE(machine.is_active_state<states::on>());
}
