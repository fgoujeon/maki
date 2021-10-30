//Copyright Florian Goujeon 2021.
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
#define STATE(NAME) \
    struct NAME \
    { \
        context& ctx; \
    };

        STATE(state0);
        STATE(state1);
        STATE(state2);
        STATE(state3);
        STATE(state4);
        STATE(state5);
        STATE(state6);
        STATE(state7);
        STATE(state8);
        STATE(state9);

#undef STATE

        struct benchmarking
        {
            void on_event(const fgfsm::event_ref& evt)
            {
                if(evt.is<events::internal_transition>())
                    ++ctx.side_effect;
            }

            context& ctx;
        };
    }

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

    using fsm = fgfsm::fsm<transition_table>;
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

#ifdef CATCH_CONFIG_ENABLE_BENCHMARKING
    BENCHMARK("process_event")
    {
        sm.process_event(events::internal_transition{});
        return ctx.side_effect;
    };
#endif
}
