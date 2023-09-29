//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
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
            static constexpr auto conf = maki::state_conf_c;

            state1(context& /*ctx*/)
            {
            }
        };
    }

    using transition_table_t = maki::transition_table
        ::add<states::state0, events::event, states::state1>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::machine_conf_c
            .set_transition_tables<transition_table_t>()
            .set_context_type<context>()
        ;
    };

    using machine_t = maki::machine<machine_def>;
}

TEST_CASE("empty_state")
{
    auto machine = machine_t{};

    machine.process_event(events::event{});
}
