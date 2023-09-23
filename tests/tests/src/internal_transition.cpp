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
        EMPTY_STATE(state0);
        EMPTY_STATE(state1);
        EMPTY_STATE(state2);
        EMPTY_STATE(state3);
        EMPTY_STATE(state4);
        EMPTY_STATE(state5);
        EMPTY_STATE(state6);
        EMPTY_STATE(state7);
        EMPTY_STATE(state8);
        EMPTY_STATE(state9);

        constexpr auto benchmarking = maki::state_c
            .add_on_event<events::internal_transition>([](context& ctx)
            {
                ++ctx.side_effect;
            })
        ;
    }

    using transition_table_t = maki::transition_table
        ::add<states::state0, events::next_state, states::state1>
        ::add<states::state1, events::next_state, states::state2>
        ::add<states::state2, events::next_state, states::state3>
        ::add<states::state3, events::next_state, states::state4>
        ::add<states::state4, events::next_state, states::state5>
        ::add<states::state5, events::next_state, states::state6>
        ::add<states::state6, events::next_state, states::state7>
        ::add<states::state7, events::next_state, states::state8>
        ::add<states::state8, events::next_state, states::state9>
        ::add<states::state9, events::next_state, states::benchmarking>
    ;

    struct machine_def
    {
        using conf = maki::machine_conf
            ::transition_tables<transition_table_t>
            ::context<context>
            ::no_run_to_completion
            ::on_exception
        ;

        void on_exception(const std::exception_ptr& /*eptr*/)
        {
        }
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
    REQUIRE(machine.is_active_state(states::benchmarking));

    machine.process_event(events::internal_transition{});
    REQUIRE(ctx.side_effect == 1);
}
