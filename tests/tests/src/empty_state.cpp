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

    using transition_table_t = awesm::transition_table
        ::add<states::state0, events::event, states::state1>
    ;

    struct machine_def
    {
        using conf = awesm::machine_conf
            ::transition_tables<transition_table_t>
            ::context<context>
        ;
    };

    using machine_t = awesm::machine<machine_def>;
}

TEST_CASE("empty_state")
{
    auto machine = machine_t{};

    machine.process_event(events::event{});
}
