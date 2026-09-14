//Copyright Florian Goujeon 2021 - 2026.
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

        constexpr auto state1 = maki::state_mold{}
            .context_c<state1_data>()
        ;
    }

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini,      states::state0)
        (states::state0, states::state1, maki::event<events::event>)
    ;

    struct machine_conf
    {
        static constexpr auto value = maki::machine_conf{}
            .transition_tables(transition_table)
            .context_a<context>()
            AOS_ASYNC_OPTS
        ;
    };

    using machine_t = maki::machine<machine_conf>;

    AOS_TEST(empty_state)
    {
        auto machine = machine_t{};

        AOS_CALL machine.AOS(process_event)(events::event{});
    }
}
