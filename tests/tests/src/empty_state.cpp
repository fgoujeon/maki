//Copyright Florian Goujeon 2021 - 2024.
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
        EMPTY_STATE(state0)

        //Check empty state without default constructor
        struct state1_data
        {
            state1_data(context& /*ctx*/)
            {
            }
        };

        constexpr auto state1 = maki::state_conf{}
            .context_c(maki::type<state1_data>)
        ;
    }

    constexpr auto make_transition_table()
    {
        return maki::transition_table{}
        (states::state0, maki::type<events::event>, states::state1)
        ;
    }

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(make_transition_table)
        .context_a(maki::type<context>)
    ;

    using machine_t = maki::make_machine<machine_conf>;
}

TEST_CASE("empty_state")
{
    auto machine = machine_t{};

    machine.process_event(events::event{});
}
