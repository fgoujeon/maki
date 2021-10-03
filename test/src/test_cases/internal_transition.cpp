//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include <fgfsm.hpp>
#include <catch2/catch.hpp>

namespace
{
    struct context
    {
        std::string output;
    };

    namespace events
    {
        struct button_press{};
    }

    namespace states
    {
        struct idle
        {
            void on_entry()
            {
                ctx.output += "on_entry;";
            }

            void on_exit()
            {
                ctx.output += "on_exit;";
            }

            context& ctx;
        };
    }

    namespace actions
    {
        struct beep
        {
            template<class StartState, class Event, class TargetState>
            void operator()(const StartState&, const Event&, const TargetState)
            {
                ctx.output += "beep;";
            }

            context& ctx;
        };
    }

    using transition_table = fgfsm::transition_table
    <
        fgfsm::row<states::idle, events::button_press, states::idle, actions::beep>
    >;

    using fsm = fgfsm::fsm<transition_table>;
}

TEST_CASE("internal transition")
{
    auto ctx = context{};
    auto sm = fsm{ctx};

    sm.process_event(events::button_press{});
    REQUIRE(ctx.output == "beep;");
}
