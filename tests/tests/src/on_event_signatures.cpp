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
    struct machine_def;

    using machine_t = maki::machine<machine_def>;

    struct context
    {
        std::string out;
    };

    namespace events
    {
        struct event0
        {
            std::string value;
        };

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
        struct state0
        {
            static constexpr auto conf = maki::state_conf_c
                .enable_on_event<events::event0, events::event1, events::event2>()
            ;

            void on_event(const events::event0& event)
            {
                ctx.out = "on_event " + event.value;
            }

            void on_event_ce(context& ctxt, const events::event1& event)
            {
                ctxt.out = "on_event_ce " + event.value;
            }

            void on_event_mce(machine_t& /*mach*/, context& ctxt, const events::event2& event)
            {
                ctxt.out = "on_event_mce " + event.value;
            }

            context& ctx;
        };
    }

    using transition_table_t = maki::transition_table
        ::add<states::state0, events::unused, maki::null>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::machine_conf_c
            .set_transition_tables<transition_table_t>()
            .set_context_type<context>()
        ;

        context& ctx;
    };
}

TEST_CASE("on_event_signatures")
{
    auto machine = machine_t{};
    auto& ctx = machine.context();

    ctx.out.clear();
    machine.process_event(events::event0{"0"});
    REQUIRE(ctx.out == "on_event 0");

    ctx.out.clear();
    machine.process_event(events::event1{"1"});
    REQUIRE(ctx.out == "on_event_ce 1");

    ctx.out.clear();
    machine.process_event(events::event2{"2"});
    REQUIRE(ctx.out == "on_event_mce 2");
}
