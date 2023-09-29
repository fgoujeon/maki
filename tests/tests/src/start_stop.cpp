//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"
#include <string>

namespace
{
    struct context
    {
        std::string out;
    };

    namespace states
    {
        struct s0
        {
            static constexpr auto conf = maki::state_conf_c
                .enable_on_entry_any()
                .enable_on_exit_any()
            ;

            void on_entry()
            {
                ctx.out += "s0::on_entry;";
            }

            void on_exit()
            {
                ctx.out += "s0::on_exit;";
            }

            context& ctx;
        };

        struct s1
        {
            static constexpr auto conf = maki::state_conf_c
                .enable_on_entry_any()
                .enable_on_exit_any()
            ;

            void on_entry()
            {
                ctx.out += "s1::on_entry;";
            }

            void on_exit()
            {
                ctx.out += "s1::on_exit;";
            }

            context& ctx;
        };
    }

    namespace events
    {
        struct button_press{};
    }

    using transition_table_t = maki::transition_table
        ::add<states::s0, maki::null,           states::s1>
        ::add<states::s1, events::button_press, states::s0>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::machine_conf_c
            .set_transition_tables<transition_table_t>()
            .set_context_type<context>()
            .disable_auto_start()
        ;
    };

    using machine_t = maki::machine<machine_def>;
}

TEST_CASE("start_stop")
{
    auto machine = machine_t{};
    auto& ctx = machine.context();

    REQUIRE(!machine.is_running());
    REQUIRE(ctx.out == "");

    machine.start();
    REQUIRE(machine.is_active_state<states::s1>());
    REQUIRE(ctx.out == "s0::on_entry;s0::on_exit;s1::on_entry;");

    ctx.out.clear();
    machine.stop();
    REQUIRE(!machine.is_running());
    REQUIRE(ctx.out == "s1::on_exit;");
}
