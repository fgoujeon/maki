//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_RESOLVE_TRANSITION_TABLE_HPP
#define AWESM_DETAIL_RESOLVE_TRANSITION_TABLE_HPP

#include "alternative.hpp"
#include "alternative_lazy.hpp"
#include "tlu.hpp"
#include "../row.hpp"
#include "../type_pattern.hpp"
#include "../transition_table.hpp"

namespace awesm::detail
{

/*
Replaces each pattern-start-state row with equivalent set of n rows, n being the
number of state types that match the pattern.
The list of state types is given as second argument to save some build time.
It's already computed at the time we call resolve_transition_table.

For example, the following resolved_transition_table_t type...:
    using transition_table = awesm::transition_table
    <
        awesm::row<state0,     event0, state1, action0, guard0>,
        awesm::row<state1,     event1, state2, action1, guard1>,
        awesm::row<awesm::any, event2, state3, action2, guard2>
    >;
    using resolved_transition_table_t =
        awesm::detail::resolve_transition_table
        <
            transition_table,
            tuple<state0, state1, state2, state3>
        >
    ;

... is equivalent to this type:
    using resolved_transition_table_t = awesm::transition_table
    <
        awesm::row<state0, event0, state1, action0, guard0>,
        awesm::row<state1, event1, state2, action1, guard1>,
        awesm::row<state0, event2, state3, action2, guard2>,
        awesm::row<state1, event2, state3, action2, guard2>,
        awesm::row<state2, event2, state3, action2, guard2>,
        awesm::row<state3, event2, state3, action2, guard2>
    >;
*/

namespace resolve_transition_table_detail
{
    template<class RowWithPattern>
    struct add_resolved_row_holder
    {
        template<class TransitionTable, class SourceState>
        using type = alternative
        <
            RowWithPattern::source_state_type::template matches<SourceState>,
            tlu::push_back
            <
                TransitionTable,
                row
                <
                    SourceState,
                    typename RowWithPattern::event_type,
                    typename RowWithPattern::target_state_type,
                    typename RowWithPattern::action_type,
                    typename RowWithPattern::guard_type
                >
            >,
            TransitionTable
        >;
    };

    /*
    Return TransitionTable with n new rows, n being the number of matching
    states.
    */
    template<class TransitionTable, class Row, class StateTypeList>
    struct add_row_with_pattern_holder
    {
        template<class = void>
        using type = tlu::left_fold
        <
            StateTypeList,
            add_resolved_row_holder<Row>::template type,
            TransitionTable
        >;
    };

    template<class TransitionTable, class Row>
    struct add_row_without_pattern_holder
    {
        template<class = void>
        using type = tlu::push_back<TransitionTable, Row>;
    };

    //We need a holder to pass StateTypeList
    template<class StateTypeList>
    struct add_row_holder
    {
        /*
        Return TransitionTable with added rows.
        Added rows are either:
        - Row as is if Row::source_state_type isn't a pattern;
        - n resolved rows otherwise (n being the number of states that match the
          pattern).
        */
        template<class TransitionTable, class Row>
        using type = alternative_lazy
        <
            std::is_base_of_v<type_pattern, typename Row::source_state_type>,
            add_row_with_pattern_holder<TransitionTable, Row, StateTypeList>,
            add_row_without_pattern_holder<TransitionTable, Row>
        >;
    };
}

template<class TransitionTable, class StateTypeList>
using resolve_transition_table = tlu::left_fold
<
    TransitionTable,
    resolve_transition_table_detail::add_row_holder<StateTypeList>::template type,
    transition_table<>
>;

} //namespace

#endif
