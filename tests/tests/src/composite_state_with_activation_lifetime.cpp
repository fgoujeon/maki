//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace composite_state_with_activation_lifetime_ns
{
    struct context
    {
        std::unique_ptr<int> pi;
    };

    namespace events
    {
        struct button_press{};

        struct color_button_press{};
    }

    namespace states
    {
        EMPTY_STATE(off)

        namespace on_ns
        {
            struct context
            {
                using parent_context_type = composite_state_with_activation_lifetime_ns::context;

                context(parent_context_type& parent):
                    i(*parent.pi)
                {
                }

                int& i;
            };

            EMPTY_STATE(emitting_red)

            namespace emitting_green_ns
            {
                struct context
                {
                    context(on_ns::context& parent):
                        i(parent.i)
                    {
                    }

                    int& i;
                };
            }

            constexpr auto emitting_green = maki::state_mold{}
                .context_c<emitting_green_ns::context>()
                .entry_action_c
                (
                    [](emitting_green_ns::context& ctx)
                    {
                        ++ctx.i;
                    }
                )
            ;

            namespace emitting_blue_ns
            {
                struct context
                {
                    context(on_ns::context& parent):
                        i(parent.i)
                    {
                    }

                    int& i;
                };
            }

            constexpr auto emitting_blue = maki::state_mold{}
                .context_c<emitting_blue_ns::context>()
                .context_lifetime(maki::state_context_lifetime::state_activity)
                .entry_action_c
                (
                    [](emitting_blue_ns::context& ctx)
                    {
                        ++ctx.i;
                    }
                )
            ;

            namespace emitting_yellow_ns
            {
                struct context
                {
                    context(on_ns::context& parent):
                        i(parent.i)
                    {
                    }

                    int& i;
                };

                constexpr auto dummy = maki::state_mold{};

                constexpr auto transition_table = maki::transition_table{}
                    (maki::ini, dummy)
                ;
            }

            constexpr auto emitting_yellow = maki::state_mold{}
                .context_c<emitting_yellow_ns::context>()
                .context_lifetime(maki::state_context_lifetime::state_activity)
                .transition_tables(emitting_yellow_ns::transition_table)
                .entry_action_c
                (
                    [](emitting_yellow_ns::context& ctx)
                    {
                        ++ctx.i;
                    }
                )
            ;

            constexpr auto transition_table = maki::transition_table{}
                (maki::ini,       emitting_red)
                (emitting_red,    emitting_green,  maki::event<events::color_button_press>)
                (emitting_green,  emitting_blue,   maki::event<events::color_button_press>)
                (emitting_blue,   emitting_yellow, maki::event<events::color_button_press>)
                (emitting_yellow, emitting_red,    maki::event<events::color_button_press>)
            ;
        }

        constexpr auto on = maki::state_mold{}
            .transition_tables(on_ns::transition_table)
            .context_c<on_ns::context>()
            .context_lifetime(maki::state_context_lifetime::state_activity)
        ;
    }

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini,   states::off)
        (states::off, states::on,  maki::event<events::button_press>)
        (states::on,  states::off, maki::event<events::button_press>)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("composite_state_with_activation_lifetime")
{
    using namespace composite_state_with_activation_lifetime_ns;

    auto machine = machine_t{};
    auto& ctx = machine.context();

    machine.start();

    ctx.pi = std::make_unique<int>(42);

    machine.process_event(events::button_press{});
    REQUIRE(machine.is<states::on>());
    REQUIRE(machine.state<states::on>().is<states::on_ns::emitting_red>());
    REQUIRE(*ctx.pi == 42);
    REQUIRE(machine.state<states::on>().context().has_value());
    REQUIRE(machine.state<states::on>().context()->i == 42);

    machine.process_event(events::color_button_press{});
    REQUIRE(machine.is<states::on>());
    REQUIRE(machine.state<states::on>().is<states::on_ns::emitting_green>());
    REQUIRE(*ctx.pi == 43);
    REQUIRE(machine.state<states::on>().context().has_value());
    REQUIRE(machine.state<states::on>().context()->i == 43);
    REQUIRE(machine.state<states::on>().substate<states::on_ns::emitting_green>().context().has_value());
    REQUIRE(machine.state<states::on>().substate<states::on_ns::emitting_green>().context()->i == 43);
    REQUIRE(!machine.state<states::on>().substate<states::on_ns::emitting_blue>().context().has_value());

    machine.process_event(events::color_button_press{});
    REQUIRE(machine.is<states::on>());
    REQUIRE(machine.state<states::on>().is<states::on_ns::emitting_blue>());
    REQUIRE(*ctx.pi == 44);
    REQUIRE(machine.state<states::on>().context().has_value());
    REQUIRE(machine.state<states::on>().context()->i == 44);
    REQUIRE(machine.state<states::on>().substate<states::on_ns::emitting_green>().context().has_value());
    REQUIRE(machine.state<states::on>().substate<states::on_ns::emitting_blue>().context().has_value());
    REQUIRE(machine.state<states::on>().substate<states::on_ns::emitting_blue>().context()->i == 44);

    machine.process_event(events::color_button_press{});
    REQUIRE(machine.is<states::on>());
    REQUIRE(machine.state<states::on>().is<states::on_ns::emitting_yellow>());
    REQUIRE(*ctx.pi == 45);
    REQUIRE(machine.state<states::on>().context().has_value());
    REQUIRE(machine.state<states::on>().context()->i == 45);
    REQUIRE(machine.state<states::on>().substate<states::on_ns::emitting_green>().context().has_value());
    REQUIRE(machine.state<states::on>().substate<states::on_ns::emitting_yellow>().context().has_value());
    REQUIRE(machine.state<states::on>().substate<states::on_ns::emitting_yellow>().context()->i == 45);

    machine.process_event(events::button_press{});
    REQUIRE(machine.is<states::off>());
    REQUIRE(!machine.state<states::on>().context().has_value());
    REQUIRE(!machine.state<states::on>().substate<states::on_ns::emitting_green>().context().has_value());
    REQUIRE(!machine.state<states::on>().substate<states::on_ns::emitting_blue>().context().has_value());
}
