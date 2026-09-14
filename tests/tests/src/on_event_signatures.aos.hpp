//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"
#include <string>

namespace AOS(on_event_signatures)
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
        using machine_ref_t =
#if AOS_ASYNC
            maki::any_async_machine_ref_e<boost::cobalt::promise, events::unused>
#else
            maki::machine_ref_e<events::unused>
#endif
        ;

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
                [](context& ctx, machine_ref_t /*mach*/, const events::event2& event)
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

    struct machine_conf
    {
        static constexpr auto value = maki::machine_conf{}
            .transition_tables(transition_table)
            .context_a<context>()
            AOS_MACHINE_OPTS
        ;
    };

    using machine_t = maki::machine<machine_conf>;

    AOS_TEST_CASE("on_event_signatures")
    {
        auto machine = machine_t{};
        auto& ctx = machine.context();

#if AOS_ASYNC
        co_await machine.async_start();
#endif

        ctx.out.clear();
        AOS_CALL machine.AOS(process_event)(events::event1{"1"});
        REQUIRE(ctx.out == "on_event_ce 1");

        ctx.out.clear();
        AOS_CALL machine.AOS(process_event)(events::event2{"2"});
        REQUIRE(ctx.out == "on_event_cme 2");
    }
}
