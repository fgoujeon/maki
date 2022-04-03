//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_DETAIL_TRANSITION_TABLE_DIGEST_HPP
#define FGFSM_DETAIL_TRANSITION_TABLE_DIGEST_HPP

#include "tlu.hpp"
#include "../any.hpp"
#include "../none.hpp"
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
        using action_tuple = std::tuple<action0, action1>;
        using guard_tuple = std::tuple<guard0, guard1>;
        using state_tuple = std::tuple<state0, state1, state2, state3>;
        static constexpr auto has_any_start_states = false;
        static constexpr auto has_none_events = false;
    };
*/

namespace transition_table_digest_detail
{
    template<class TList, class U>
    using push_back_unique_if_not_any_or_none = tlu::push_back_if
    <
        TList,
        U,
        !tlu::contains<TList, U> && !std::is_same_v<U, any> && !std::is_same_v<U, none>
    >;

    struct initial_digest
    {
        using action_tuple = std::tuple<>;
        using guard_tuple = std::tuple<>;
        using state_tuple = std::tuple<>;
        static constexpr auto has_any_start_states = false;
        static constexpr auto has_none_events = false;
    };

    template<class Digest, class Row>
    struct add_row_to_digest
    {
        using action_tuple = push_back_unique_if_not_any_or_none
        <
            typename Digest::action_tuple,
            typename Row::action
        >;

        using guard_tuple = push_back_unique_if_not_any_or_none
        <
            typename Digest::guard_tuple,
            typename Row::guard
        >;

        using state_tuple = push_back_unique_if_not_any_or_none
        <
            push_back_unique_if_not_any_or_none
            <
                typename Digest::state_tuple,
                typename Row::start_state
            >,
            typename Row::target_state
        >;

        static constexpr auto has_any_start_states =
            Digest::has_any_start_states ||
            std::is_same_v<typename Row::start_state, any>
        ;

        static constexpr auto has_none_events =
            Digest::has_none_events ||
            std::is_same_v<typename Row::event, none>
        ;
    };
}

template<class TransitionTable>
using transition_table_digest = tlu::left_fold
<
    TransitionTable,
    transition_table_digest_detail::add_row_to_digest,
    transition_table_digest_detail::initial_digest
>;

} //namespace

#endif
