//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include "common.hpp"
#include <string>

namespace
{
    struct context
    {
    };

    namespace events
    {
        struct event{};
    }

    namespace states
    {
        EMPTY_STATE(state0);

        //Check empty state without default constructor
        struct state1
        {
            using conf = awesm::state_conf;

            state1(context& /*ctx*/)
            {
            }
        };
    }

    using sm_transition_table = awesm::transition_table
        ::add<states::state0, events::event, states::state1>
    ;

    struct sm_def
    {
        using conf = awesm::sm_conf
            ::transition_tables<sm_transition_table>
            ::context<context>
        ;
    };

    using sm_t = awesm::sm<sm_def>;
}

TEST_CASE("empty_state")
{
    auto sm = sm_t{};

    sm.process_event(events::event{});
}
