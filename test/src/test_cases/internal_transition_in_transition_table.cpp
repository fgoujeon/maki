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
        struct power_button_press{};
        struct beep_button_press{};
    }

    namespace states
    {
        struct idle
        {
            void on_entry(const fgfsm::any_cref&)
            {
                ctx.out += "idle::on_entry;";
            }

            void on_event(const fgfsm::any_cref&)
            {
                ctx.out += "idle::on_event;";
            }

            void on_exit(const fgfsm::any_cref&)
            {
                ctx.out += "idle::on_exit;";
            }

            context& ctx;
        };

        struct running
        {
            void on_entry(const fgfsm::any_cref&)
            {
                ctx.out += "running::on_entry;";
            }

            void on_event(const fgfsm::any_cref&)
            {
                ctx.out += "running::on_event;";
            }

            void on_exit(const fgfsm::any_cref&)
            {
                ctx.out += "running::on_exit;";
            }

            context& ctx;
        };
    }

    namespace actions
    {
        struct beep
        {
            void operator()(fgfsm::unused, const events::beep_button_press&, fgfsm::unused)
            {
                ctx.out += "beep;";
            }

            context& ctx;
        };
    };

    using transition_table = fgfsm::transition_table
    <
        fgfsm::row<states::idle,    events::power_button_press, states::running>,
        fgfsm::row<states::running, events::power_button_press, states::idle>,
        fgfsm::row<states::running, events::beep_button_press,  fgfsm::none,      actions::beep>
    >;

    using fsm = fgfsm::fsm<transition_table>;
}

TEST_CASE("internal transition in transition table")
{
    auto ctx = context{};
    auto sm = fsm{ctx};

    sm.process_event(events::power_button_press{});
    REQUIRE(ctx.out == "idle::on_event;idle::on_exit;running::on_entry;");

    ctx.out.clear();
    sm.process_event(events::beep_button_press{});
    REQUIRE(ctx.out == "running::on_event;beep;");
}
