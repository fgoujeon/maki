//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"
#include <string>

namespace internal_transition_in_transition_table_ns
{
    struct context
    {
        std::string out;
    };

    namespace events
    {
        struct should_trigger_transition_in_state{};
        struct should_trigger_transition_in_tt{};
    }

    namespace states
    {
        constexpr auto idle = maki::state_conf{}
            .internal_action_c<events::should_trigger_transition_in_state>
            (
                [](context& ctx)
                {
                    ctx.out += "idle::internal_action;";
                }
            )
        ;

        constexpr auto running = maki::state_conf{};
    }

    constexpr auto transition_table = maki::transition_table{}
        (states::idle,    states::running, maki::event<events::should_trigger_transition_in_state>)
        (states::idle,    states::running, maki::event<events::should_trigger_transition_in_tt>)
        (states::running, states::idle, maki::event<events::should_trigger_transition_in_tt>)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("transition_priority")
{
    using namespace internal_transition_in_transition_table_ns;

    auto machine = machine_t{};
    auto& ctx = machine.context();

    REQUIRE(machine.is<states::idle>());

    ctx.out.clear();
    machine.process_event(events::should_trigger_transition_in_state{});
    REQUIRE(machine.is<states::idle>());
    REQUIRE(ctx.out == "idle::internal_action;");

    ctx.out.clear();
    machine.process_event(events::should_trigger_transition_in_tt{});
    REQUIRE(machine.is<states::running>());
    REQUIRE(ctx.out == "");

    ctx.out.clear();
    machine.process_event(events::should_trigger_transition_in_tt{});
    REQUIRE(machine.is<states::idle>());
    REQUIRE(ctx.out == "");
}
