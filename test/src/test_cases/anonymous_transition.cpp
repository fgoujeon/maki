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
        struct s0{};
        struct s1{};
        struct s2{};
        struct s3{};
        struct s4{};
    };

    struct fsm_conf: fgfsm::fsm_configuration
    {
        using transition_table_t = fgfsm::transition_table
        <
            fgfsm::row<states::s0, events::go_on, states::s1>,
            fgfsm::row<states::s1, fgfsm::none,   states::s2>,
            fgfsm::row<states::s2, events::go_on, states::s3>,
            fgfsm::row<states::s3, fgfsm::none,   states::s4>,
            fgfsm::row<states::s4, fgfsm::none,   states::s0>
        >;
    };

    using fsm = fgfsm::fsm<fsm_conf>;
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
