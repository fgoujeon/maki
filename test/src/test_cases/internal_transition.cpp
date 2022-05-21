//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
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

    struct sm_configuration: awesm::sm_configuration
    {
        using transition_table = awesm::transition_table
        <
            awesm::row<states::state0, events::next_state, states::state1>,
            awesm::row<states::state1, events::next_state, states::state2>,
            awesm::row<states::state2, events::next_state, states::state3>,
            awesm::row<states::state3, events::next_state, states::state4>,
            awesm::row<states::state4, events::next_state, states::state5>,
            awesm::row<states::state5, events::next_state, states::state6>,
            awesm::row<states::state6, events::next_state, states::state7>,
            awesm::row<states::state7, events::next_state, states::state8>,
            awesm::row<states::state8, events::next_state, states::state9>,
            awesm::row<states::state9, events::next_state, states::benchmarking>
        >;

        static constexpr auto enable_run_to_completion = false;
    };

    using sm = awesm::sm<sm_configuration>;
}

TEST_CASE("internal transition")
{
    auto ctx = context{};
    auto machine = sm{ctx};

    for(auto i = 0; i < 10; ++i)
        machine.process_event(events::next_state{});
    REQUIRE(machine.is_active_state<states::benchmarking>());

    machine.process_event(events::internal_transition{});
    REQUIRE(ctx.side_effect == 1);
}
