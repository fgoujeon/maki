//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include <catch2/catch.hpp>
#include <string>

namespace
{
    struct context
    {
        std::string out;
    };

    namespace states
    {
        struct on
        {
            void on_entry()
            {
                ctx.out += "on::on_entry;";
            }

            void on_exit()
            {
                ctx.out += "on::on_exit;";
            }

            context& ctx;
        };

        struct off
        {
            void on_entry()
            {
                ctx.out += "off::on_entry;";
            }

            void on_exit()
            {
                ctx.out += "off::on_exit;";
            }

            context& ctx;
        };
    }

    namespace events
    {
        struct button_press{};
    }

    struct sm_configuration: awesm::simple_sm_configuration
    {
        using transition_table = awesm::transition_table
        <
            awesm::row<states::off, events::button_press, states::on>,
            awesm::row<states::on,  events::button_press, states::off>
        >;
    };

    using sm_t = awesm::simple_sm<sm_configuration>;
}

TEST_CASE("start_stop")
{
    auto ctx = context{};
    auto sm = sm_t{ctx};

    REQUIRE(ctx.out == "");

    sm.start();
    REQUIRE(sm.is_active_state<states::off>());
    REQUIRE(ctx.out == "off::on_entry;");

    ctx.out.clear();
    sm.process_event(events::button_press{});
    REQUIRE(sm.is_active_state<states::on>());
    REQUIRE(ctx.out == "off::on_exit;on::on_entry;");

    ctx.out.clear();
    sm.stop();
    REQUIRE(sm.is_active_state<states::on>());
    REQUIRE(ctx.out == "on::on_exit;");
}
