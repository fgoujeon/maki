//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TRANSITION_TABLE_DIGEST_HPP
#define MAKI_DETAIL_TRANSITION_TABLE_DIGEST_HPP

#include "tuple.hpp"
#include "integer_constant_sequence.hpp"
#include "iseq.hpp"
#include "../states.hpp"
#include "../null.hpp"
#include "tlu/left_fold.hpp"
#include <type_traits>

namespace maki::detail
{

/*
Creates a sequence of transition indexes. The target state molds of these
transitions are all the state molds referred to by the transition table.

For example, the following digest type...:
    using transition_table = maki::transition_table{}
        (maki::ini,  state0)
        (state0,     state1, event0)
        (state1,     state2, event1, null,    guard0)
        (state2,     state3, event2, action0)
        (state3,     state0, event3, action1, guard1)
    >;
    using digest = maki::detail::transition_table_digest<transition_table>;

... is equivalent to this type:
    struct digest
    {
        using unique_target_state_mold_iseq = maki::detail::iseq<0, 1, 2, 3>;
    };
*/

namespace transition_table_digest_detail
{
    struct initial_digest
    {
        using unique_target_state_mold_iseq = iseq<>;
        static constexpr auto has_completion_transitions = false;
    };

    template<const auto& TransitionTable, class TargetStateMoldIndexSequence, int StateMoldIndex>
    struct has_target_state_mold;

    template<const auto& TransitionTable, int... TargetStateMoldIndexes, int StateMoldIndex>
    struct has_target_state_mold<TransitionTable, iseq<TargetStateMoldIndexes...>, StateMoldIndex>
    {
        static constexpr bool value =
            (
                equals
                (
                    tuple_get<TargetStateMoldIndexes>(impl_of(TransitionTable)).target_state_mold,
                    tuple_get<StateMoldIndex>(impl_of(TransitionTable)).target_state_mold
                ) || ...
            )
        ;
    };

    template<const auto& TransitionTable>
    struct add_transition_to_digest_holder
    {
        template<class Digest, int Index>
        struct add_transition_to_digest_impl
        {
            static constexpr const auto& target_state_mold =
                tuple_get<Index>(impl_of(TransitionTable)).target_state_mold
            ;

            /*
            We must add target state to list of states unless:
            - it's already in the list;
            - it's `fin`;
            - it's `null`;
            - it's `undefined`.
            */
            static constexpr auto must_add_target_state =
                !has_target_state_mold
                <
                    TransitionTable,
                    typename Digest::unique_target_state_mold_iseq,
                    Index
                >::value &&
                !equals(target_state_mold, state_molds::fin) &&
                !equals(target_state_mold, null) &&
                !equals(target_state_mold, undefined)
            ;

            using unique_target_state_mold_iseq =
                iseq_push_back_if_t
                <
                    typename Digest::unique_target_state_mold_iseq,
                    Index,
                    must_add_target_state
                >
            ;

            static constexpr auto has_completion_transitions =
                Digest::has_completion_transitions ||
                (
                    Index != 0 &&
                    is_null_v
                    <
                        std::decay_t<decltype(tuple_get<Index>(impl_of(TransitionTable)).evt)>
                    >
                )
            ;
        };

        template<class Digest, class IndexConstant>
        using add_transition_to_digest = add_transition_to_digest_impl
        <
            Digest,
            IndexConstant::value
        >;
    };
}

template<const auto& TransitionTable>
using transition_table_digest = tlu::left_fold_t
<
    make_integer_constant_sequence<int, impl_of(TransitionTable).size>,
    transition_table_digest_detail::add_transition_to_digest_holder
    <
        TransitionTable
    >::template add_transition_to_digest,
    transition_table_digest_detail::initial_digest
>;

} //namespace

#endif
