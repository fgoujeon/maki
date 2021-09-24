//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include <fgfsm.hpp>
#include <catch2/catch.hpp>

namespace
{
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

    using transition_table = fgfsm::transition_table
    <
        fgfsm::row<states::s0, events::go_on, states::s1>,
        fgfsm::row<states::s1, fgfsm::none,   states::s2>,
        fgfsm::row<states::s2, events::go_on, states::s3>,
        fgfsm::row<states::s3, fgfsm::none,   states::s4>,
        fgfsm::row<states::s4, fgfsm::none,   states::s0>
    >;

    using fsm = fgfsm::fsm<transition_table>;
}

TEST_CASE("anonymous transition")
{
    auto sm = fsm{};

    sm.process_event(events::go_on{});
    REQUIRE(sm.is_current_state<states::s2>());

    sm.process_event(events::go_on{});
    REQUIRE(sm.is_current_state<states::s0>());
}
