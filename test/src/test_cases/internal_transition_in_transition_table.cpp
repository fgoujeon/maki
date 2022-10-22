//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include "common/catch.hpp"
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
            void on_entry(awesm::whatever)
            {
                ctx.out += "idle::on_entry;";
            }

            void on_event(awesm::whatever)
            {
                ctx.out += "idle::on_event;";
            }

            void on_exit(awesm::whatever)
            {
                ctx.out += "idle::on_exit;";
            }

            context& ctx;
        };

        struct running
        {
            void on_entry(awesm::whatever)
            {
                ctx.out += "running::on_entry;";
            }

            void on_event(awesm::whatever)
            {
                ctx.out += "running::on_event;";
            }

            void on_exit(awesm::whatever)
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
            void execute(awesm::whatever)
            {
                ctx.out += "beep;";
            }

            context& ctx;
        };
    };

    struct sm_transition_table
    {
        using type = awesm::transition_table
        <
            awesm::row<states::idle,    events::power_button_press, states::running>,
            awesm::row<states::running, events::power_button_press, states::idle>,
            awesm::row<states::running, events::beep_button_press,  awesm::none,      actions::beep>
        >;
    };

    using sm_t = awesm::simple_sm<sm_transition_table>;
}

TEST_CASE("internal_transition_in_transition_table")
{
    auto ctx = context{};
    auto sm = sm_t{ctx};

    sm.start();
    REQUIRE(ctx.out == "idle::on_entry;");

    ctx.out.clear();
    sm.process_event(events::power_button_press{});
    REQUIRE(ctx.out == "idle::on_event;idle::on_exit;running::on_entry;");

    ctx.out.clear();
    sm.process_event(events::beep_button_press{});
    REQUIRE(ctx.out == "running::on_event;beep;");
}
