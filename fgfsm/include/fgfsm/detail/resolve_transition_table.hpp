//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_DETAIL_RESOLVE_TRANSITION_TABLE_HPP
#define FGFSM_DETAIL_RESOLVE_TRANSITION_TABLE_HPP

#include "alternative.hpp"
#include "tlu.hpp"
#include "../row.hpp"
#include "../any.hpp"
#include "../transition_table.hpp"

namespace fgfsm::detail
{

/*
Replaces each 'any'-start-state row with equivalent set of n rows, n being the
number of distinct state types.
The list of state types is given as second argument to save some build time.
It's already computed at the time we call resolve_transition_table.

For example, the following resolved_transition_table_t type...:
    using transition_table = fgfsm::transition_table
    <
        fgfsm::row<state0,     event0, state1, action0, guard0>,
        fgfsm::row<state1,     event1, state2, action1, guard1>,
        fgfsm::row<fgfsm::any, event2, state3, action2, guard2>
    >;
    using resolved_transition_table_t =
        fgfsm::detail::resolve_transition_table
        <
            transition_table,
            tuple<state0, state1, state2, state3>
        >
    ;

... is equivalent to this type:
    using resolved_transition_table_t = fgfsm::transition_table
    <
        fgfsm::row<state0, event0, state1, action0, guard0>,
        fgfsm::row<state1, event1, state2, action1, guard1>,
        fgfsm::row<state0, event2, state3, action2, guard2>,
        fgfsm::row<state1, event2, state3, action2, guard2>,
        fgfsm::row<state2, event2, state3, action2, guard2>,
        fgfsm::row<state3, event2, state3, action2, guard2>
    >;
*/

//has_any_start_state
namespace resolve_transition_table_detail
{
    template<class Row>
    struct has_any_start_state_helper;

    template
    <
        class StartState,
        class Event,
        class TargetState,
        class Action,
        class Guard
    >
    struct has_any_start_state_helper
    <
        row
        <
            StartState,
            Event,
            TargetState,
            Action,
            Guard
        >
    >
    {
        static constexpr auto value = std::is_same_v<StartState, fgfsm::any>;
    };

    template<class Row>
    constexpr auto has_any_start_state = has_any_start_state_helper<Row>::value;
}

namespace resolve_transition_table_detail
{
    template<class OriginalRow>
    struct add_resolved_row
    {
        template<class TransitionTable, class StartState>
        using call = tlu::push_back
        <
            TransitionTable,
            row
            <
                StartState,
                typename OriginalRow::event,
                typename OriginalRow::target_state,
                typename OriginalRow::action,
                typename OriginalRow::guard
            >
        >;
    };

    template<class StateTypeList>
    struct helper
    {
        template<class TransitionTable, class Row>
        using add_any_row = tlu::left_fold
        <
            StateTypeList,
            add_resolved_row<Row>::template call,
            TransitionTable
        >;

        template<class TransitionTable, class Row>
        using add_row = alternative
        <
            has_any_start_state<Row>,
            add_any_row<TransitionTable, Row>,
            tlu::push_back<TransitionTable, Row>
        >;
    };
}

template<class TransitionTable, class StateTypeList>
using resolve_transition_table = tlu::left_fold
<
    TransitionTable,
    resolve_transition_table_detail::helper<StateTypeList>::template add_row,
    transition_table<>
>;

} //namespace

#endif
