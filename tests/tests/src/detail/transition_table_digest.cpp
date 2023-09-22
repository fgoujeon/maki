//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include "../common.hpp"
#include <maki.hpp>

namespace
{
    struct context{};

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

    using transition_table_t = maki::transition_table
        ::add<state0,     event0, state1>
        ::add<state1,     event1, state2,  maki::noop, guard0>
        ::add<state2,     event2, state3,  action0>
        ::add<state3,     event3, state0,  action1,     guard1>
        ::add<maki::any, event3, state0>
    ;

    struct machine_def
    {
        using conf = maki::machine_conf
            ::transition_tables<transition_table_t>
            ::context<context>
        ;
    };

    using machine_t = maki::machine<machine_def>;

    using region_path_t = maki::region_path<machine_t>;

    using digest_t = maki::detail::transition_table_digest<transition_table_t, machine_t>;

    using state_tuple_t = maki::detail::type_list<state0, state1, state2, state3>;
}

TEST_CASE("detail::transition_table_digest")
{
    REQUIRE(std::is_same_v<digest_t::state_type_list, state_tuple_t>);
    REQUIRE(!digest_t::has_null_events);
}