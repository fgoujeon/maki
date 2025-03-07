//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"
#include <string>

namespace on_exception_ns
{
    struct context
    {
        int always_zero = 0;
        std::string out;
    };

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
                }
            )
        ;

        constexpr auto on = maki::state_builder{}
            .entry_action_c
            (
                [](context& ctx)
                {
                    ctx.out += "on::on_entry;";

                    if(ctx.always_zero == 0) //We need this to avoid "unreachable code" warnings
                    {
                        throw std::runtime_error{"test;"};
                    }
                }
            )
            .internal_action_ce<maki::events::exception>
            (
                [](context& ctx, const maki::events::exception& event)
                {
                    try
                    {
                        std::rethrow_exception(event.eptr);
                    }
                    catch(const std::exception& e)
                    {
                        ctx.out += "default;";
                        ctx.out += e.what();
                    }
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

    namespace events
    {
        struct button_press{};
    }

    constexpr auto transition_table = maki::transition_table{}
        (maki::init,  states::off)
        (states::off, states::on,  maki::event<events::button_press>)
        (states::on,  states::off, maki::event<events::button_press>)
    ;

    constexpr auto default_machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
    ;

    using default_sm_t = maki::machine<default_machine_conf>;

    constexpr auto custom_machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
        .exception_hook_mx
        (
            [](auto& mach, const std::exception_ptr& eptr)
            {
                try
                {
                    std::rethrow_exception(eptr);
                }
                catch(const std::exception& e)
                {
                    mach.context().out += "custom;";
                    mach.context().out += e.what();
                }
            }
        )
    ;

    using custom_sm_t = maki::machine<custom_machine_conf>;
}

TEST_CASE("on_exception")
{
    using namespace on_exception_ns;

    {
        auto machine = default_sm_t{};
        auto& ctx = machine.context();

        machine.start();
        ctx.out.clear();

        machine.process_event(events::button_press{});
        REQUIRE(machine.is<states::on>());
        REQUIRE(ctx.out == "off::on_exit;on::on_entry;default;test;");
    }

    {
        auto machine = custom_sm_t{};
        auto& ctx = machine.context();

        machine.start();
        ctx.out.clear();

        machine.process_event(events::button_press{});
        REQUIRE(machine.is<states::on>());
        REQUIRE(ctx.out == "off::on_exit;on::on_entry;custom;test;");
    }
}
