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
        struct off{};

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

    struct action
    {
        void execute(const events::e1&)
        {
            ctx.out += "execute(e1);";
        }

        void execute()
        {
            ctx.out += "execute();";
        }

        context& ctx;
    };

    struct guard
    {
        bool check(const events::e1&)
        {
            ctx.out += "check(e1);";
            return true;
        }

        bool check()
        {
            ctx.out += "check();";
            return true;
        }

        context& ctx;
    };

    struct fsm_configuration: fgfsm::fsm_configuration
    {
        using transition_table_t = fgfsm::transition_table
        <
            fgfsm::row<states::off, events::e1, states::on,  action, guard>,
            fgfsm::row<states::off, events::e2, states::on,  action, guard>,
            fgfsm::row<states::on,  events::e1, states::off, action, guard>,
            fgfsm::row<states::on,  events::e2, states::off, action, guard>
        >;
    };

    using fsm = fgfsm::fsm<fsm_configuration>;
}

TEST_CASE("nullary_member_functions")
{
    auto ctx = context{};
    auto sm = fsm{ctx};

    ctx.out.clear();
    sm.process_event(events::e1{});
    REQUIRE(ctx.out == "check(e1);execute(e1);on_entry(e1);");

    ctx.out.clear();
    sm.process_event(events::e1{});
    REQUIRE(ctx.out == "check(e1);on_exit(e1);execute(e1);");

    ctx.out.clear();
    sm.process_event(events::e2{});
    REQUIRE(ctx.out == "check();execute();on_entry();");

    ctx.out.clear();
    sm.process_event(events::e2{});
    REQUIRE(ctx.out == "check();on_exit();execute();");
}
