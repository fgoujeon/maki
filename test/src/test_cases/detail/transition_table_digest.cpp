//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include <fgfsm/row.hpp>
#include <fgfsm/state_transition_table.hpp>
#include <fgfsm/none.hpp>
#include <fgfsm/detail/state_transition_table_digest.hpp>
#include <catch2/catch.hpp>
#include <tuple>

namespace
{
    struct state0{};
    struct state1{};
    struct state2{};
    struct state3{};

    struct event0{};
    struct event1{};
    struct event2{};
    struct event3{};

    struct action0{};
    struct action1{};

    struct guard0{};
    struct guard1{};

    using state_transition_table = fgfsm::state_transition_table
    <
        fgfsm::row<state0, event0, state1>,
        fgfsm::row<state1, event1, state2, fgfsm::none, guard0>,
        fgfsm::row<state2, event2, state3, action0>,
        fgfsm::row<state3, event3, state0, action1,     guard1>
    >;

    using digest = fgfsm::detail::state_transition_table_digest<state_transition_table>;

    using action_tuple = std::tuple<fgfsm::none, action0, action1>;
    using guard_tuple = std::tuple<fgfsm::none, guard0, guard1>;
    using state_tuple = std::tuple<state0, state1, state2, state3>;
}

TEST_CASE("detail::transition_table_digest")
{
    REQUIRE(std::is_same_v<digest::action_tuple, action_tuple>);
    REQUIRE(std::is_same_v<digest::guard_tuple, guard_tuple>);
    REQUIRE(std::is_same_v<digest::state_tuple, state_tuple>);
}
