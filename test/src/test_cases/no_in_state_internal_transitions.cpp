//Copyright Florian Goujeon 2021 - 2022.
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
            using conf = awesm::state_conf<>;

            void on_entry()
            {
            }

            void on_event(const events::internal_transition&)
            {
                ++ctx.i;
            }

            context& ctx;
        };
    }

    using sm_transition_table = awesm::transition_table
    <
        awesm::row<states::single, events::self_transition, states::single>
    >;

    struct sm_def
    {
        using conf = awesm::sm_conf
        <
            sm_transition_table,
            awesm::sm_options::disable_in_state_internal_transitions
        >;
    };

    using sm_t = awesm::sm<sm_def>;
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
