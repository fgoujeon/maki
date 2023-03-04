//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
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

        struct benchmarking
        {
            using conf = awesm::state_conf
                ::on_event<events::internal_transition>
            ;

            void on_event(const events::internal_transition&)
            {
                ++ctx.side_effect;
            }

            context& ctx;
        };
    }

    auto sm_transition_table()
    {
        return awesm::transition_table
            .add<states::state0, events::next_state, states::state1>
            .add<states::state1, events::next_state, states::state2>
            .add<states::state2, events::next_state, states::state3>
            .add<states::state3, events::next_state, states::state4>
            .add<states::state4, events::next_state, states::state5>
            .add<states::state5, events::next_state, states::state6>
            .add<states::state6, events::next_state, states::state7>
            .add<states::state7, events::next_state, states::state8>
            .add<states::state8, events::next_state, states::state9>
            .add<states::state9, events::next_state, states::benchmarking>
        ;
    }

    struct sm_def
    {
        using conf = awesm::sm_conf
            ::transition_table<sm_transition_table>
            ::context<context>
            ::no_run_to_completion
            ::on_exception
        ;

        void on_exception(const std::exception_ptr& /*eptr*/)
        {
        }
    };

    using sm_t = awesm::sm<sm_def>;
}

TEST_CASE("internal transition")
{
    auto sm = sm_t{};
    auto& ctx = sm.get_context();

    sm.start();

    for(auto i = 0; i < 10; ++i)
    {
        sm.process_event(events::next_state{});
    }
    REQUIRE(sm.is_active_state<states::benchmarking>());

    sm.process_event(events::internal_transition{});
    REQUIRE(ctx.side_effect == 1);
}
