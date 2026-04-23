//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TRANSITION_TABLE_DIGEST_HPP
#define MAKI_DETAIL_TRANSITION_TABLE_DIGEST_HPP

#include "constant.hpp"
#include "tuple.hpp"
#include "integer_constant_sequence.hpp"
#include "integer_constant_sequence_tuple.hpp"
#include "type_list.hpp"
#include "../states.hpp"
#include "../null.hpp"
#include "tlu/push_back_if.hpp"
#include "tlu/left_fold.hpp"
#include <type_traits>

namespace maki::detail
{

/*
Creates a set of tuples containing all the action types, guard types and state
types of a given transition_table.

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
        using state_def_type_list = maki::detail::type_list_t<state0, state1, state2, state3>;
    };
*/

template<class StateIdConstantList, bool HasCompletionTransition>
struct transition_table_digest
{
    using state_id_constant_list = StateIdConstantList;
    static constexpr auto has_completion_transitions = HasCompletionTransition;
};

namespace digest_transition_table_detail
{
    template<const auto& TransitionTable>
    struct add_to_digest
    {
        template<class Digest, std::size_t Index>
        static constexpr auto call
        (
            const type_t<Digest> /*ignored*/,
            const std::integral_constant<std::size_t, Index> /*ignored*/
        )
        {
            /*
            We must add target state to list of states unless:
            - it's already in the list;
            - it's `fin`;
            - it's `null`;
            - it's `undefined`.
            */
            constexpr auto must_add_target_state =
                !tlu::contains_v
                <
                    typename Digest::state_id_constant_list,
                    constant_t<tuple_get<Index>(impl_of(TransitionTable)).target_state_mold>
                > &&
                !equals(tuple_get<Index>(impl_of(TransitionTable)).target_state_mold, state_molds::fin) &&
                !equals(tuple_get<Index>(impl_of(TransitionTable)).target_state_mold, null) &&
                !equals(tuple_get<Index>(impl_of(TransitionTable)).target_state_mold, undefined)
            ;

            using state_id_constant_list = tlu::push_back_if_t
            <
                typename Digest::state_id_constant_list,
                constant_t<tuple_get<Index>(impl_of(TransitionTable)).target_state_mold>,
                must_add_target_state
            >;

            constexpr auto has_completion_transitions =
                Digest::has_completion_transitions ||
                (
                    Index != 0 &&
                    is_null_v
                    <
                        std::decay_t<decltype(tuple_get<Index>(impl_of(TransitionTable)).evt)>
                    >
                )
            ;

            return type<transition_table_digest<state_id_constant_list, has_completion_transitions>>;
        }
    };
}

template<const auto& TransitionTable>
constexpr auto digest_transition_table()
{
    const auto transition_table_index_tuple =
        integer_constant_sequence_tuple
        <
            std::size_t,
            impl_of(TransitionTable).size
        >
    ;

    return transition_table_index_tuple.left_fold
    (
        []
        (
            const auto& digest_type_holder,
            const auto& index_constant
        )
        {
            return digest_transition_table_detail::add_to_digest<TransitionTable>::call
            (
                digest_type_holder,
                index_constant
            );
        },
        type
        <
            transition_table_digest
            <
                type_list_t<>,
                false
            >
        >
    );
}

} //namespace

#endif
