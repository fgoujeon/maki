//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include <fgfsm.hpp>
#include <catch2/catch.hpp>

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
        struct state0{};
        struct state1{};
        struct state2{};
        struct state3{};
        struct state4{};
        struct state5{};
        struct state6{};
        struct state7{};
        struct state8{};
        struct state9{};

        struct benchmarking
        {
            void on_entry()
            {
            }

            void on_event(const events::internal_transition&)
            {
                ++ctx.side_effect;
            }

            void on_exit()
            {
            }

            context& ctx;
        };
    }

    struct fsm_configuration: fgfsm::fsm_configuration
    {
        using transition_table = fgfsm::transition_table
        <
            fgfsm::row<states::state0, events::next_state, states::state1>,
            fgfsm::row<states::state1, events::next_state, states::state2>,
            fgfsm::row<states::state2, events::next_state, states::state3>,
            fgfsm::row<states::state3, events::next_state, states::state4>,
            fgfsm::row<states::state4, events::next_state, states::state5>,
            fgfsm::row<states::state5, events::next_state, states::state6>,
            fgfsm::row<states::state6, events::next_state, states::state7>,
            fgfsm::row<states::state7, events::next_state, states::state8>,
            fgfsm::row<states::state8, events::next_state, states::state9>,
            fgfsm::row<states::state9, events::next_state, states::benchmarking>
        >;

        static constexpr auto enable_run_to_completion = false;
    };

    using fsm = fgfsm::fsm<fsm_configuration>;
}

TEST_CASE("internal transition")
{
    auto ctx = context{};
    auto sm = fsm{ctx};

    for(auto i = 0; i < 10; ++i)
        sm.process_event(events::next_state{});
    REQUIRE(sm.is_active_state<states::benchmarking>());

    sm.process_event(events::internal_transition{});
    REQUIRE(ctx.side_effect == 1);
}
