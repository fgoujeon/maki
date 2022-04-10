//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include <fgfsm.hpp>
#include <catch2/catch.hpp>
#include <string>

namespace
{
    struct context
    {
        std::string out;
    };

    namespace events
    {
        struct e1
        {
        };

        struct e2
        {
        };
    }

    namespace states
    {
        FGFSM_SIMPLE_STATE(off)

        struct on
        {
            void on_entry(const events::e1&)
            {
                ctx.out += "on_entry(e1);";
            }

            void on_entry()
            {
                ctx.out += "on_entry();";
            }

            void on_event(const fgfsm::any_cref&)
            {
            }

            void on_exit(const events::e1&)
            {
                ctx.out += "on_exit(e1);";
            }

            void on_exit()
            {
                ctx.out += "on_exit();";
            }

            context& ctx;
        };
    }

    using transition_table = fgfsm::transition_table
    <
        fgfsm::row<states::off, events::e1, states::on>,
        fgfsm::row<states::off, events::e2, states::on>,
        fgfsm::row<states::on,  events::e1, states::off>,
        fgfsm::row<states::on,  events::e2, states::off>
    >;

    using fsm = fgfsm::fsm<transition_table>;
}

TEST_CASE("nullary_on_entry_exit")
{
    auto ctx = context{};
    auto sm = fsm{ctx};

    ctx.out.clear();
    sm.process_event(events::e1{});
    REQUIRE(ctx.out == "on_entry(e1);");

    ctx.out.clear();
    sm.process_event(events::e1{});
    REQUIRE(ctx.out == "on_exit(e1);");

    ctx.out.clear();
    sm.process_event(events::e2{});
    REQUIRE(ctx.out == "on_entry();");

    ctx.out.clear();
    sm.process_event(events::e2{});
    REQUIRE(ctx.out == "on_exit();");
}
