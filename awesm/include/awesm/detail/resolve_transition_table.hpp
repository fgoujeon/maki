//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_RESOLVE_TRANSITION_TABLE_HPP
#define AWESM_DETAIL_RESOLVE_TRANSITION_TABLE_HPP

#include "alternative.hpp"
#include "tlu.hpp"
#include "../type_patterns.hpp"
#include "../transition_table.hpp"

namespace awesm::detail
{

/*
Replaces each pattern-start-state transition with equivalent set of n
transitions, n being the number of state types that match the pattern.
The list of state types is given as second argument to save some build time.
It's already computed at the time we call resolve_transition_table.

For example, the following resolved_transition_table_t type...:
    using transition_table = awesm::transition_table
    <
        awesm::transition<state0,     event0, state1, action0, guard0>,
        awesm::transition<state1,     event1, state2, action1, guard1>,
        awesm::transition<awesm::any, event2, state3, action2, guard2>
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
        awesm::transition<state0, event0, state1, action0, guard0>,
        awesm::transition<state1, event1, state2, action1, guard1>,
        awesm::transition<state0, event2, state3, action2, guard2>,
        awesm::transition<state1, event2, state3, action2, guard2>,
        awesm::transition<state2, event2, state3, action2, guard2>,
        awesm::transition<state3, event2, state3, action2, guard2>
    >;
*/

namespace resolve_transition_table_detail
{
#ifdef _MSC_VER
    template<class TransitionWithPattern>
    struct add_resolved_transition_holder;

    template<class SourceState, class Event, class TargetState, const auto& Action, const auto& Guard>
    struct add_resolved_transition_holder<transition<SourceState, Event, TargetState, Action, Guard>>
    {
        template<class TransitionTable, class State>
        using type = alternative_t
        <
            SourceState::template matches<State>,
            tlu::push_back_t
            <
                TransitionTable,
                transition<State, Event, TargetState, Action, Guard>
            >,
            TransitionTable
        >;
    };
#else
    template<class TransitionWithPattern>
    struct add_resolved_transition_holder
    {
        template<class TransitionTable, class State>
        using type = alternative_t
        <
            TransitionWithPattern::source_state_type::template matches<State>,
            tlu::push_back_t
            <
                TransitionTable,
                transition
                <
                    State,
                    typename TransitionWithPattern::event_type,
                    typename TransitionWithPattern::target_state_type,
                    TransitionWithPattern::get_action(),
                    TransitionWithPattern::get_guard()
                >
            >,
            TransitionTable
        >;
    };
#endif

    /*
    Return TransitionTable with n new transitions, n being the number of
    matching states.
    */
    template<class TransitionTable, class Transition, class StateTypeList>
    struct add_transition_with_pattern_holder
    {
        template<bool = true> //Dummy template for lazy evaluation
        using type = tlu::left_fold_t
        <
            StateTypeList,
            add_resolved_transition_holder<Transition>::template type,
            TransitionTable
        >;
    };

    template<class TransitionTable, class Transition>
    struct add_transition_without_pattern_holder
    {
        template<bool = true> //Dummy template for lazy evaluation
        using type = tlu::push_back_t<TransitionTable, Transition>;
    };

    //We need a holder to pass StateTypeList
    template<class StateTypeList>
    struct add_transition_holder
    {
        /*
        Return TransitionTable with added transitions.
        Added transitions are either:
        - Transition as is if Transition::source_state_type isn't a pattern;
        - n resolved transitions otherwise (n being the number of states that
          match the pattern).
        */
        template<class TransitionTable, class Transition>
        using type = typename alternative_t
        <
            is_type_pattern_v<typename Transition::source_state_type>,
            add_transition_with_pattern_holder<TransitionTable, Transition, StateTypeList>,
            add_transition_without_pattern_holder<TransitionTable, Transition>
        >::template type<>;
    };
}

template<class TransitionTable, class StateTypeList, bool HasSourceStatePatterns>
struct resolve_transition_table;

template<class TransitionTable, class StateTypeList>
struct resolve_transition_table<TransitionTable, StateTypeList, true>
{
    using type = tlu::left_fold_t
    <
        TransitionTable,
        resolve_transition_table_detail::add_transition_holder<StateTypeList>::template type,
        transition_table_t<>
    >;
};

template<class TransitionTable, class StateTypeList>
struct resolve_transition_table<TransitionTable, StateTypeList, false>
{
    using type = TransitionTable;
};

template<class TransitionTable, class StateTypeList, bool HasSourceStatePatterns>
using resolve_transition_table_t = typename resolve_transition_table
<
    TransitionTable,
    StateTypeList,
    HasSourceStatePatterns
>::type;

} //namespace

#endif
