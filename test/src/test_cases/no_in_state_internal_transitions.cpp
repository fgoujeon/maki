//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include "common/catch.hpp"

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

    using sm_transition_table = awesm::transition_table
    <
        awesm::row<states::single, events::self_transition, states::single>
    >;

    struct sm_def
    {
        using transition_tables = awesm::transition_table_list<sm_transition_table>;
    };

    using sm_t = awesm::sm
    <
        sm_def,
        awesm::sm_options::disable_in_state_internal_transitions
    >;
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
