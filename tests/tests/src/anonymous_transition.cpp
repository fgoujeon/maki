//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace
{
    struct context{};

    namespace events
    {
        struct go_on{};
    }

    namespace states
    {
        EMPTY_STATE(s0)
        EMPTY_STATE(s1)
        EMPTY_STATE(s2)
        EMPTY_STATE(s3)
        EMPTY_STATE(s4)
    }

    constexpr auto transition_table = maki::empty_transition_table
        .add_c<states::s0, events::go_on, states::s1>
        .add_c<states::s1, maki::null,    states::s2>
        .add_c<states::s2, events::go_on, states::s3>
        .add_c<states::s3, maki::null,    states::s4>
        .add_c<states::s4, maki::null,    states::s0>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::default_machine_conf
            .transition_tables(transition_table)
            .context<context>()
        ;
    };

    using machine_t = maki::machine<machine_def>;
}

TEST_CASE("anonymous transition")
{
    auto machine = machine_t{};

    machine.start();

    machine.process_event(events::go_on{});
    REQUIRE(machine.is_active_state<states::s2>());

    machine.process_event(events::go_on{});
    REQUIRE(machine.is_active_state<states::s0>());
}
