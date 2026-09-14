//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace
{
    struct context
    {
        std::string events_processed_by_c;
    };

    namespace events
    {
        struct e1{};
        struct e2{};
        struct e3{};
        struct e4{};
        struct e5{};
    }

    namespace states
    {
        constexpr auto a = maki::state_mold{};

        constexpr auto b = maki::state_mold{}
            .defer<events::e2>();

        constexpr auto c = maki::state_mold{}
            .internal_action_c<events::e2>(
                [](context& ctx)
                {
                    ctx.events_processed_by_c += "e2";
                })
            .internal_action_c<events::e4>(
                [](context& ctx)
                {
                    ctx.events_processed_by_c += "e4";
                })
            .internal_action_c<events::e5>(
                [](context& ctx)
                {
                    ctx.events_processed_by_c += "e5";
                });
    }

    namespace actions
    {
        constexpr auto emit_e2_e3_e4 = maki::action_m(
            [](auto& mach) -> AOS_VOID
            {
                AOS_CALL mach.AOS(process_event)(events::e2{});
                AOS_CALL mach.AOS(process_event)(events::e3{});
                AOS_CALL mach.AOS(process_event)(events::e4{});
                AOS_RETURN;
            });

        constexpr auto emit_e5 = maki::action_m(
            [](auto& mach) -> AOS_VOID
            {
                AOS_CALL mach.AOS(process_event)(events::e5{});
                AOS_RETURN;
            });
    }

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini, states::a)
        (states::a, states::b, maki::event<events::e1>, actions::emit_e2_e3_e4)
        (states::b, states::c, maki::event<events::e3>, actions::emit_e5)
    ;

    struct machine_conf
    {
        static constexpr auto value = maki::machine_conf{}
            .transition_tables(transition_table)
            .context_a<context>()
            AOS_ASYNC_OPTS
        ;
    };

    using machine_t = maki::machine<machine_conf>;

    AOS_TEST(defer_rtc)
    {
        auto machine = machine_t{};

        AOS_CALL machine.AOS(start)();

        AOS_CALL machine.AOS(process_event)(events::e1{});
        REQUIRE(machine.context().events_processed_by_c == "e2e4e5");
    }
}
