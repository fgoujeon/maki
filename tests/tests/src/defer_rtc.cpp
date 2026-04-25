//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace defer_rtc_ns
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
            [](auto& mach)
            {
                mach.process_event(events::e2{});
                mach.process_event(events::e3{});
                mach.process_event(events::e4{});
            });

        constexpr auto emit_e5 = maki::action_m(
            [](auto& mach)
            {
                mach.process_event(events::e5{});
            });
    }

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini, states::a)
        (states::a, states::b, maki::event<events::e1>, actions::emit_e2_e3_e4)
        (states::b, states::c, maki::event<events::e3>, actions::emit_e5)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("defer_rtc")
{
    using namespace defer_rtc_ns;

    auto machine = machine_t{};

    machine.start();

    machine.process_event(events::e1{});
    REQUIRE(machine.context().events_processed_by_c == "e2e4e5");
}
