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

    FGFSM_SIMPLE_STATE(state0);
    FGFSM_SIMPLE_STATE(state1);
    FGFSM_SIMPLE_STATE(state2);
    FGFSM_SIMPLE_STATE(state3);

    struct event0{};
    struct event1{};
    struct event2{};

    struct action0
    {
        void execute(const fgfsm::any_cref&){}
        context& ctx;
    };

    struct action1
    {
        void execute(const fgfsm::any_cref&){}
        context& ctx;
    };

    struct action2
    {
        void execute(const fgfsm::any_cref&){}
        context& ctx;
    };

    struct guard0
    {
        bool check(const fgfsm::any_cref&){return true;}
        context& ctx;
    };

    struct guard1
    {
        bool check(const fgfsm::any_cref&){return true;}
        context& ctx;
    };

    struct guard2
    {
        bool check(const fgfsm::any_cref&){return true;}
        context& ctx;
    };
}

TEST_CASE("detail::resolve_transition_table")
{
    using transition_table = fgfsm::transition_table
    <
        fgfsm::row<state0,     event0, state1, action0, guard0>,
        fgfsm::row<fgfsm::any, event0, state1, action0, guard0>,
        fgfsm::row<state1,     event1, state2, action1, guard1>,
        fgfsm::row<fgfsm::any, event2, state3, action2, guard2>
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
        //copied
        fgfsm::row<state0, event0, state1, action0, guard0>,

        //resolved
        fgfsm::row<state0, event0, state1, action0, guard0>,
        fgfsm::row<state1, event0, state1, action0, guard0>,
        fgfsm::row<state2, event0, state1, action0, guard0>,
        fgfsm::row<state3, event0, state1, action0, guard0>,

        //copied
        fgfsm::row<state1, event1, state2, action1, guard1>,

        //resolved
        fgfsm::row<state0, event2, state3, action2, guard2>,
        fgfsm::row<state1, event2, state3, action2, guard2>,
        fgfsm::row<state2, event2, state3, action2, guard2>,
        fgfsm::row<state3, event2, state3, action2, guard2>
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
