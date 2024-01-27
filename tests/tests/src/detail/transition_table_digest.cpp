//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include "../common.hpp"
#include <maki.hpp>

namespace transition_table_digest_ns
{
    struct context{};

    EMPTY_STATE(state0)
    EMPTY_STATE(state1)
    EMPTY_STATE(state2)
    EMPTY_STATE(state3)

    struct event0{};
    struct event1{};
    struct event2{};
    struct event3{};

    void action0(){}
    void action1(){}

    bool guard0(){return true;}
    bool guard1(){return true;}

    constexpr auto transition_table = maki::transition_table{}
        (state0,    maki::type<event0>, state1)
        (state1,    maki::type<event1>, state2, maki::null, guard0)
        (state2,    maki::type<event2>, state3, action0)
        (state3,    maki::type<event3>, state0, action1,    guard1)
        (maki::any, maki::type<event3>, state0)
    ;

    constexpr auto transition_tuple = maki::detail::rows(transition_table);

    constexpr auto digest = maki::detail::transition_table_digest<transition_tuple>;

    constexpr auto expected_state_conf_ptrs = maki::detail::make_tuple
    (
        maki::detail::distributed_construct,
        &state0,
        &state1,
        &state2,
        &state3
    );
}

TEST_CASE("detail::transition_table_digest")
{
    using namespace transition_table_digest_ns;
    REQUIRE(digest.state_conf_ptrs == expected_state_conf_ptrs);
    REQUIRE(!digest.has_null_events);
}
