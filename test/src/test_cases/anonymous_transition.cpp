//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include "common.hpp"

namespace
{
    struct context{};

    struct events
    {
        struct go_on{};
    };

    struct states
    {
        EMPTY_STATE(s0);
        EMPTY_STATE(s1);
        EMPTY_STATE(s2);
        EMPTY_STATE(s3);
        EMPTY_STATE(s4);
    };

    auto sm_transition_table()
    {
        return awesm::transition_table
            .add<states::s0, events::go_on, states::s1>
            .add<states::s1, awesm::null,   states::s2>
            .add<states::s2, events::go_on, states::s3>
            .add<states::s3, awesm::null,   states::s4>
            .add<states::s4, awesm::null,   states::s0>
        ;
    }

    struct sm_def
    {
        using conf = awesm::sm_conf<sm_transition_table, context>;
    };

    using sm_t = awesm::sm<sm_def>;
}

TEST_CASE("anonymous transition")
{
    auto sm = sm_t{};

    sm.start();

    sm.process_event(events::go_on{});
    REQUIRE(sm.is_active_state<states::s2>());

    sm.process_event(events::go_on{});
    REQUIRE(sm.is_active_state<states::s0>());
}
