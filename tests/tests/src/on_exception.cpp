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

        struct exception
        {
            std::exception_ptr eptr;
        };
    }

    constexpr auto log_exception_default = maki::action_ce
    (
        [](context& ctx, const events::exception& event)
        {
            try
            {
                std::rethrow_exception(event.eptr);
            }
            catch(const std::exception& e)
            {
                ctx.out += e.what();
            }
        }
    );

    constexpr auto transition_table = maki::transition_table{}
        (maki::init,          states::off)
        (states::off,         states::on,  maki::event<events::button_press>)
        (states::on,          states::off, maki::event<events::button_press>)
        (maki::transitioning, maki::null,  maki::event<events::exception>, log_exception_default)
    ;

    constexpr auto default_machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
    ;

    using default_sm_t = maki::machine<default_machine_conf>;

    constexpr auto custom_machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
        .catch_mx
        (
            [](auto& mach, const std::exception_ptr& eptr)
            {
                mach.process_event(events::exception{eptr});
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

        CHECK_THROWS(machine.process_event(events::button_press{}));
        CHECK(machine.is<maki::transitioning>());
        CHECK(ctx.out == "off::on_exit;on::on_entry;");
    }

    {
        auto machine = custom_sm_t{};
        auto& ctx = machine.context();

        machine.start();
        ctx.out.clear();

        machine.process_event(events::button_press{});
        CHECK(machine.is<maki::transitioning>());
        CHECK(ctx.out == "off::on_exit;on::on_entry;test;");
    }
}
