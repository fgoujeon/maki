//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"
#include <string>

namespace on_event_signatures_ns
{
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
        constexpr auto state0 = maki::state_mold{}
            .internal_action_ce<events::event1>
            (
                [](context& ctx, const events::event1& event)
                {
                    ctx.out = "on_event_ce " + event.value;
                }
            )
            .internal_action_cme<events::event2>
            (
                [](context& ctx, maki::machine_ref_e<events::unused> /*mach*/, const events::event2& event)
                {
                    ctx.out = "on_event_cme " + event.value;
                }
            )
        ;
    }

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini,      states::state0)
        (states::state0, maki::null, maki::event<events::unused>)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("on_event_signatures")
{
    using namespace on_event_signatures_ns;

    auto machine = machine_t{};
    auto& ctx = machine.context();

    ctx.out.clear();
    machine.process_event(events::event1{"1"});
    REQUIRE(ctx.out == "on_event_ce 1");

    ctx.out.clear();
    machine.process_event(events::event2{"2"});
    REQUIRE(ctx.out == "on_event_cme 2");
}
