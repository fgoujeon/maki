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
    struct machine_conf_holder;

    using machine_t = maki::machine<machine_conf_holder>;

    struct context
    {
        std::string out;
    };

    namespace events
    {
        struct event1
        {
            std::string value;
        };

        struct event2
        {
            std::string value;
        };

        struct unused
        {
        };
    }

    namespace states
    {
        constexpr auto state0 = maki::state_conf{}
            .internal_action_ce<events::event1>
            (
                [](context& ctx, const events::event1& event)
                {
                    ctx.out = "on_event_ce " + event.value;
                }
            )
            .internal_action_ce<events::event2>
            (
                [](context& ctx, const events::event2& event)
                {
                    ctx.out = "on_event_mce " + event.value;
                }
            )
        ;
    }

    constexpr auto transition_table = maki::transition_table{}
        .add_c<states::state0, events::unused, maki::null_c>
    ;

    struct machine_conf_holder
    {
        static constexpr auto conf = maki::machine_conf{}
            .transition_tables(transition_table)
            .context<context>()
        ;

        context& ctx;
    };
}

TEST_CASE("on_event_signatures")
{
    auto machine = machine_t{};
    auto& ctx = machine.context();

    ctx.out.clear();
    machine.process_event(events::event1{"1"});
    REQUIRE(ctx.out == "on_event_ce 1");

    ctx.out.clear();
    machine.process_event(events::event2{"2"});
    REQUIRE(ctx.out == "on_event_mce 2");
}
