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

    struct state0{};
    struct state1{};
    struct state2{};
    struct state3
    {
        int i;
    };

    struct event0{};
    struct event1{};
    struct event2{};

    struct action0
    {
        void execute(){}
        context& ctx;
    };

    struct action1
    {
        void execute(){}
        context& ctx;
    };

    struct action2
    {
        void execute(){}
        context& ctx;
    };

    struct guard0
    {
        bool check(){return true;}
        context& ctx;
    };

    struct guard1
    {
        bool check(){return true;}
        context& ctx;
    };

    struct guard2
    {
        bool check(){return true;}
        context& ctx;
    };
}

TEST_CASE("detail::resolve_transition_table")
{
    using transition_table = fgfsm::transition_table
    <
        fgfsm::row<state0, event0, state1, action0, guard0>,
        fgfsm::row<state1, event1, state2, action1, guard1>,
        fgfsm::row<state2, event2, state3, action2, guard2>,

        //pattern 1
        fgfsm::row<fgfsm::any, event0, state0, action0, guard0>,

        //pattern 2
        fgfsm::row<fgfsm::any_of<state0, state2>, event0, state0, action0, guard0>,

        //pattern 3
        fgfsm::row<fgfsm::any_but<state0, state2>, event0, state0, action0, guard0>,

        //pattern 4
        fgfsm::row<fgfsm::any_if<std::is_empty>, event0, state0, action0, guard0>,

        //pattern 5
        fgfsm::row<fgfsm::any_if_not<std::is_empty>, event0, state0, action0, guard0>
    >;

    using resolved_transition_table_t =
        fgfsm::detail::resolve_transition_table
        <
            transition_table,
            std::tuple<state0, state1, state2, state3>
        >
    ;

    using expected_resolved_transition_table_t = fgfsm::transition_table
    <
        fgfsm::row<state0, event0, state1, action0, guard0>,
        fgfsm::row<state1, event1, state2, action1, guard1>,
        fgfsm::row<state2, event2, state3, action2, guard2>,

        //pattern 1
        fgfsm::row<state0, event0, state0, action0, guard0>,
        fgfsm::row<state1, event0, state0, action0, guard0>,
        fgfsm::row<state2, event0, state0, action0, guard0>,
        fgfsm::row<state3, event0, state0, action0, guard0>,

        //pattern 2
        fgfsm::row<state0, event0, state0, action0, guard0>,
        fgfsm::row<state2, event0, state0, action0, guard0>,

        //pattern 3
        fgfsm::row<state1, event0, state0, action0, guard0>,
        fgfsm::row<state3, event0, state0, action0, guard0>,

        //pattern 4
        fgfsm::row<state0, event0, state0, action0, guard0>,
        fgfsm::row<state1, event0, state0, action0, guard0>,
        fgfsm::row<state2, event0, state0, action0, guard0>,

        //pattern 5
        fgfsm::row<state3, event0, state0, action0, guard0>
    >;

    REQUIRE
    (
        std::is_same_v
        <
            resolved_transition_table_t,
            expected_resolved_transition_table_t
        >
    );
}
