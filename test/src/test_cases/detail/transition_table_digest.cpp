//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include <fgfsm/row.hpp>
#include <fgfsm/transition_table.hpp>
#include <fgfsm/none.hpp>
#include <fgfsm/detail/transition_table_digest.hpp>
#include <catch2/catch.hpp>
#include <tuple>

namespace
{
    struct state0{};
    struct state1{};
    struct state2{};

    struct event0{};
    struct event1{};
    struct event2{};

    using transition_table_t = fgfsm::transition_table
    <
        fgfsm::row<state0,     event0,      state1>,
        fgfsm::row<state1,     fgfsm::none, state2>,
        fgfsm::row<state2,     event1,      fgfsm::none>,
        fgfsm::row<fgfsm::any, event2,      state0>
    >;

    using digest = fgfsm::detail::transition_table_digest
    <
        transition_table_t
    >;

    using state_tuple = std::tuple<state0, state1, state2>;
    using event_tuple = std::tuple<event0, fgfsm::none, event1, event2>;
}

TEST_CASE("detail::transition_table_digest")
{
    REQUIRE(std::is_same_v<digest::state_tuple, state_tuple>);
    REQUIRE(std::is_same_v<digest::event_tuple, event_tuple>);
}
