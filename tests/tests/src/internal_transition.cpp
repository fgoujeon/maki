//Copyright Florian Goujeon 2021 - 2023.
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
        int side_effect = 0;
    };

    namespace events
    {
        struct internal_transition{};
        struct next_state{};
    }

    namespace states
    {
        EMPTY_STATE(state0)
        EMPTY_STATE(state1)
        EMPTY_STATE(state2)
        EMPTY_STATE(state3)
        EMPTY_STATE(state4)
        EMPTY_STATE(state5)
        EMPTY_STATE(state6)
        EMPTY_STATE(state7)
        EMPTY_STATE(state8)
        EMPTY_STATE(state9)

        constexpr auto benchmarking = maki::state_conf{}
            .internal_action_c<maki::type_c<events::internal_transition>>
            (
                [](context& ctx)
                {
                    ++ctx.side_effect;
                }
            )
        ;
    }

    constexpr auto transition_table = maki::transition_table{}
        (states::state0, maki::type_c<events::next_state>, states::state1)
        (states::state1, maki::type_c<events::next_state>, states::state2)
        (states::state2, maki::type_c<events::next_state>, states::state3)
        (states::state3, maki::type_c<events::next_state>, states::state4)
        (states::state4, maki::type_c<events::next_state>, states::state5)
        (states::state5, maki::type_c<events::next_state>, states::state6)
        (states::state6, maki::type_c<events::next_state>, states::state7)
        (states::state7, maki::type_c<events::next_state>, states::state8)
        (states::state8, maki::type_c<events::next_state>, states::state9)
        (states::state9, maki::type_c<events::next_state>, states::benchmarking)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context<context>()
        .run_to_completion(false)
    ;

    using machine_t = maki::make_machine<machine_conf>;
}

TEST_CASE("internal transition")
{
    auto machine = machine_t{};
    auto& ctx = machine.context();

    machine.start();

    for(auto i = 0; i < 10; ++i)
    {
        machine.process_event(events::next_state{});
    }
    REQUIRE(machine.active_state<states::benchmarking>());

    machine.process_event(events::internal_transition{});
    REQUIRE(ctx.side_effect == 1);
}
