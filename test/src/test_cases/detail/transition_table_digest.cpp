//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm/row.hpp>
#include <awesm/transition_table.hpp>
#include <awesm/region_path.hpp>
#include <awesm/detail/transition_table_digest.hpp>
#include <awesm/detail/sm_object_holder_tuple.hpp>
#include "../common.hpp"

namespace
{
    EMPTY_STATE(state0);
    EMPTY_STATE(state1);
    EMPTY_STATE(state2);
    EMPTY_STATE(state3);

    struct event0{};
    struct event1{};
    struct event2{};
    struct event3{};

    void action0(){}
    void action1(){}

    bool guard0(){return true;}
    bool guard1(){return true;}

    using transition_table = awesm::transition_table
    <
        awesm::row<state0,     event0, state1>,
        awesm::row<state1,     event1, state2,  awesm::noop, guard0>,
        awesm::row<state2,     event2, state3,  action0>,
        awesm::row<state3,     event3, state0,  action1,     guard1>,
        awesm::row<awesm::any, event3, state0>
    >;

    struct sm{};

    using region_path_t = awesm::region_path<awesm::region_path_element<sm, 0>>;

    using digest_t = awesm::detail::transition_table_digest<region_path_t, transition_table>;

    using state_tuple_t = awesm::detail::type_list<awesm::detail::null_state, state0, state1, state2, state3>;
}

TEST_CASE("detail::transition_table_digest")
{
    REQUIRE(std::is_same_v<digest_t::state_tuple_type, state_tuple_t>);
    REQUIRE(digest_t::has_source_state_patterns);
    REQUIRE(!digest_t::has_null_events);
}
