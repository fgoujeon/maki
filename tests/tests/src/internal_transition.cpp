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

        constexpr auto benchmarking = maki::state_conf_c
            .internal_action_c<events::internal_transition>
            (
                [](context& ctx)
                {
                    ++ctx.side_effect;
                }
            )
        ;
    }

    constexpr auto transition_table = maki::transition_table_c
        .add_c<states::state0, events::next_state, states::state1>
        .add_c<states::state1, events::next_state, states::state2>
        .add_c<states::state2, events::next_state, states::state3>
        .add_c<states::state3, events::next_state, states::state4>
        .add_c<states::state4, events::next_state, states::state5>
        .add_c<states::state5, events::next_state, states::state6>
        .add_c<states::state6, events::next_state, states::state7>
        .add_c<states::state7, events::next_state, states::state8>
        .add_c<states::state8, events::next_state, states::state9>
        .add_c<states::state9, events::next_state, states::benchmarking>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::machine_conf_c
            .transition_tables(transition_table)
            .context<context>()
            .run_to_completion(false)
        ;
    };

    using machine_t = maki::machine<machine_def>;
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
