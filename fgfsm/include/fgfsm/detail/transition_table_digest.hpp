//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_DETAIL_TRANSITION_TABLE_DIGEST_HPP
#define FGFSM_DETAIL_TRANSITION_TABLE_DIGEST_HPP

#include "tlu.hpp"
#include "../transition_table.hpp"
#include <tuple>

namespace fgfsm::detail
{

/*
Creates a set of tuples containing all the action types, guard types and state
types of a given transition_table.

For example, the following digest type...:
    using transition_table = fgfsm::transition_table
    <
        fgfsm::row<state0, event0, state1>,
        fgfsm::row<state1, event1, state2, fgfsm::none, guard0>,
        fgfsm::row<state2, event2, state3, action0>,
        fgfsm::row<state3, event3, state0, action1,     guard1>
    >;
    using digest = fgfsm::detail::transition_table_digest<transition_table>;

... is equivalent to this type:
    struct digest
    {
        using action_tuple = fgfsm::detail::tuple<fgfsm::none, action0, action1>;
        using guard_tuple = fgfsm::detail::tuple<fgfsm::none, guard0, guard1>;
        using state_tuple = fgfsm::detail::tuple<state0, state1, state2, state3>;
    };
*/

namespace transition_table_digest_detail
{
    template<class ActionTuple, class GuardTuple, class StateTuple, class... Rows>
    struct helper;

    template<class ActionTuple, class GuardTuple, class StateTuple, class Row, class... Rows>
    struct helper<ActionTuple, GuardTuple, StateTuple, Row, Rows...>:
        helper
        <
            tlu::push_back_unique<ActionTuple, typename Row::action>,
            tlu::push_back_unique<GuardTuple, typename Row::guard>,
            tlu::push_back_unique
            <
                tlu::push_back_unique<StateTuple, typename Row::start_state>,
                typename Row::target_state
            >,
            Rows...
        >
    {
    };

    //terminal case
    template<class ActionTuple, class GuardTuple, class StateTuple>
    struct helper<ActionTuple, GuardTuple, StateTuple>
    {
        using action_tuple = ActionTuple;
        using guard_tuple = GuardTuple;
        using state_tuple = StateTuple;
    };
}

template<class TransitionTable>
struct transition_table_digest;

template<class... Rows>
struct transition_table_digest<transition_table<Rows...>>:
    transition_table_digest_detail::helper
    <
        std::tuple<>,
        std::tuple<>,
        std::tuple<>,
        Rows...
    >
{
};

} //namespace

#endif
