//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace basic_exception_ns
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
        constexpr auto off = maki::state_builder{}
            .entry_action_c
            (
                [](context& ctx)
                {
                    ctx.out += "off::on_entry;";
                }
            )
            .exit_action_c
            (
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

        constexpr auto on = maki::state_builder{}
            .entry_action_c
            (
                [](context& ctx)
                {
                    ctx.out += "on::on_entry;";
                }
            )
            .exit_action_c
            (
                [](context& ctx)
                {
                    ctx.out += "on::on_exit;";
                }
            )
        ;
    }

    namespace actions
    {
        constexpr auto unreachable = maki::action_c([](context& ctx)
        {
            ctx.out += "unreachable;";
        });
    }

    constexpr auto transition_table = maki::transition_table{}
        (maki::init,  states::off)
        (states::off, states::on,  maki::event<events::button_press>)
        (states::off, states::on,  maki::event<events::button_press>, actions::unreachable)
        (states::on,  states::off, maki::event<events::button_press>, actions::unreachable)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("basic_exception")
{
    using namespace basic_exception_ns;

    auto machine = machine_t{};

    REQUIRE(machine.is<states::off>());
    REQUIRE(machine.context().out == "off::on_entry;");

    machine.context().out.clear();
    CHECK_THROWS(machine.process_event(events::button_press{}));
    REQUIRE(machine.is<maki::undefined>());
    REQUIRE(machine.context().out == "off::on_exit;");
}
