//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace
{
    struct context
    {
        bool exception_thrown = false;
        std::string out;
    };

    namespace events
    {
        struct button_press{};
    }

    namespace states
    {
        constexpr auto off = maki::state_conf{}
            .entry_action_c
            (
                maki::any,
                [](context& ctx)
                {
                    ctx.out += "off::on_entry;";
                }
            )
            .exit_action_c
            (
                maki::any,
                [](context& ctx)
                {
                    ctx.out += "off::on_exit;";

                    if(!ctx.exception_thrown)
                    {
                        ctx.exception_thrown = true;
                        throw std::runtime_error{"error"};
                    }
                }
            )
        ;

        constexpr auto on = maki::state_conf{}
            .entry_action_c
            (
                maki::any,
                [](context& ctx)
                {
                    ctx.out += "on::on_entry;";
                }
            )
            .exit_action_c
            (
                maki::any,
                [](context& ctx)
                {
                    ctx.out += "on::on_exit;";
                }
            )
        ;
    }

    namespace actions
    {
        void unreachable(context& ctx)
        {
            ctx.out += "unreachable;";
        }
    }

    constexpr auto make_transition_table()
    {
        return maki::transition_table{}
        (states::off, maki::type<events::button_press>, states::on)
        (states::off, maki::type<events::button_press>, states::on,  actions::unreachable)
        (states::on,  maki::type<events::button_press>, states::off, actions::unreachable)
        ;
    }

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(make_transition_table)
        .context_a(maki::type<context>)
    ;

    using machine_t = maki::make_machine<machine_conf>;
}

TEST_CASE("basic_exception")
{
    auto machine = machine_t{};

    REQUIRE(machine.active_state<states::off>());
    REQUIRE(machine.context().out == "off::on_entry;");

    machine.context().out.clear();
    machine.process_event(events::button_press{});
    REQUIRE(machine.active_state<states::off>());
    REQUIRE(machine.context().out == "off::on_exit;");
}
