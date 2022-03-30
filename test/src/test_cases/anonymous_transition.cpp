//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include <fgfsm.hpp>
#include <catch2/catch.hpp>

namespace
{
    struct context{};

    struct events
    {
        struct go_on{};
    };

    struct states
    {
        FGFSM_SIMPLE_STATE(s0)
        FGFSM_SIMPLE_STATE(s1)
        FGFSM_SIMPLE_STATE(s2)
        FGFSM_SIMPLE_STATE(s3)
        FGFSM_SIMPLE_STATE(s4)
    };

    using transition_table = fgfsm::transition_table
    <
        fgfsm::row<states::s0, events::go_on, states::s1>,
        fgfsm::row<states::s1, fgfsm::none,   states::s2>,
        fgfsm::row<states::s2, events::go_on, states::s3>,
        fgfsm::row<states::s3, fgfsm::none,   states::s4>,
        fgfsm::row<states::s4, fgfsm::none,   states::s0>
    >;

    struct fsm_configuration: fgfsm::fsm_configuration
    {
        using transition_table = ::transition_table;
    };

    using fsm = fgfsm::fsm<fsm_configuration>;
}

TEST_CASE("anonymous transition")
{
    auto ctx = context{};
    auto sm = fsm{ctx};

    sm.process_event(events::go_on{});
    REQUIRE(sm.is_active_state<states::s2>());

    sm.process_event(events::go_on{});
    REQUIRE(sm.is_active_state<states::s0>());
}
