//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include "../common.hpp"

namespace
{
    struct context{};

    EMPTY_STATE(state0);
    EMPTY_STATE(state1);
    EMPTY_STATE(state2);
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
    using transition_table = awesm::transition_table
    <
        awesm::row<state0, event0, state1, action0, guard0>,
        awesm::row<state1, event1, state2, action1, guard1>,
        awesm::row<state2, event2, state3, action2, guard2>,

        //pattern 1
        awesm::row<awesm::any, event0, state0, action0, guard0>,

        //pattern 2
        awesm::row<awesm::any_of<state0, state2>, event0, state0, action0, guard0>,

        //pattern 3
        awesm::row<awesm::any_but<state0, state2>, event0, state0, action0, guard0>,

        //pattern 4
        awesm::row<awesm::any_if<std::is_empty>, event0, state0, action0, guard0>,

        //pattern 5
        awesm::row<awesm::any_if_not<std::is_empty>, event0, state0, action0, guard0>
    >;

    using resolved_transition_table_t =
        awesm::detail::resolve_transition_table
        <
            transition_table,
            std::tuple<state0, state1, state2, state3>
        >
    ;

    using expected_resolved_transition_table_t = awesm::transition_table
    <
        awesm::row<state0, event0, state1, action0, guard0>,
        awesm::row<state1, event1, state2, action1, guard1>,
        awesm::row<state2, event2, state3, action2, guard2>,

        //pattern 1
        awesm::row<state0, event0, state0, action0, guard0>,
        awesm::row<state1, event0, state0, action0, guard0>,
        awesm::row<state2, event0, state0, action0, guard0>,
        awesm::row<state3, event0, state0, action0, guard0>,

        //pattern 2
        awesm::row<state0, event0, state0, action0, guard0>,
        awesm::row<state2, event0, state0, action0, guard0>,

        //pattern 3
        awesm::row<state1, event0, state0, action0, guard0>,
        awesm::row<state3, event0, state0, action0, guard0>,

        //pattern 4
        awesm::row<state0, event0, state0, action0, guard0>,
        awesm::row<state1, event0, state0, action0, guard0>,
        awesm::row<state2, event0, state0, action0, guard0>,

        //pattern 5
        awesm::row<state3, event0, state0, action0, guard0>
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
