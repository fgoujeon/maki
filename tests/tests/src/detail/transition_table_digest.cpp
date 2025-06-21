//Copyright Florian Goujeon 2021 - 2025.
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

    constexpr auto action0 = maki::action_v([]{});
    constexpr auto action1 = maki::action_v([]{});

    constexpr auto guard0 = maki::guard_v([]{return true;});
    constexpr auto guard1 = maki::guard_v([]{return true;});

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini,        state0)
        (state0,           state1, maki::event<event0>)
        (state1,           state2, maki::event<event1>, maki::null, guard0)
        (state2,           state3, maki::event<event2>, action0)
        (state3,           state0, maki::event<event3>, action1,    guard1)
        (maki::all_states, state0, maki::event<event3>)
    ;

    constexpr auto transition_tuple = maki::detail::rows(transition_table);

    using digest_t = maki::detail::transition_table_digest<transition_tuple>;

    using state_mold_ptr_constant_list = maki::detail::type_list_t
    <
        maki::detail::constant_t<&state0>,
        maki::detail::constant_t<&state1>,
        maki::detail::constant_t<&state2>,
        maki::detail::constant_t<&state3>
    >;
}

TEST_CASE("detail::transition_table_digest")
{
    using namespace transition_table_digest_ns;
    REQUIRE(std::is_same_v<digest_t::state_id_constant_list, state_mold_ptr_constant_list>);
}
