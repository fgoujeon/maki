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
        int i = 0;
    };

    namespace events
    {
        struct self_transition{};

        struct internal_transition{};
    }

    namespace states
    {
        struct single
        {
            void on_entry()
            {
            }

            void on_event(const events::internal_transition&)
            {
                ++ctx.i;
            }

            void on_exit()
            {
            }

            context& ctx;
        };
    }

    struct sm_configuration: awesm::simple_sm_configuration
    {
        using transition_table = awesm::transition_table
        <
            awesm::row<states::single, events::self_transition, states::single>
        >;

        static constexpr auto enable_in_state_internal_transitions = false;
    };

    using sm_t = awesm::simple_sm<sm_configuration>;
}

TEST_CASE("no_in_state_internal_transitions")
{
    auto ctx = context{};
    auto sm = sm_t{ctx};

    sm.start();

    //Shall have no effect
    sm.process_event(events::internal_transition{});
    REQUIRE(ctx.i == 0);
}
