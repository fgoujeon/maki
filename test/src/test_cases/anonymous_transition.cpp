//Copyright Florian Goujeon 2021.
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
        struct s0
        {
            void on_entry(){}
            void on_exit(){}
            context& ctx;
        };

        struct s1
        {
            void on_entry(){}
            void on_exit(){}
            context& ctx;
        };

        struct s2
        {
            void on_entry(){}
            void on_exit(){}
            context& ctx;
        };

        struct s3
        {
            void on_entry(){}
            void on_exit(){}
            context& ctx;
        };

        struct s4
        {
            void on_entry(){}
            void on_exit(){}
            context& ctx;
        };
    };

    using state_transition_table = fgfsm::state_transition_table
    <
        fgfsm::row<states::s0, events::go_on, states::s1>,
        fgfsm::row<states::s1, fgfsm::none,   states::s2>,
        fgfsm::row<states::s2, events::go_on, states::s3>,
        fgfsm::row<states::s3, fgfsm::none,   states::s4>,
        fgfsm::row<states::s4, fgfsm::none,   states::s0>
    >;

    using fsm = fgfsm::fsm<state_transition_table>;
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
