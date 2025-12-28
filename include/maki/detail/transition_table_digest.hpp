//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TRANSITION_TABLE_DIGEST_HPP
#define MAKI_DETAIL_TRANSITION_TABLE_DIGEST_HPP

#include "constant.hpp"
#include "tuple.hpp"
#include "integer_constant_sequence.hpp"
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

namespace transition_table_digest_detail
{
    struct initial_digest
    {
        using state_id_constant_list = type_list_t<>;
        static constexpr auto has_completion_transitions = false;
    };

    template<class Digest, class TransitionPtrConstant>
    struct add_transition_to_digest
    {
        /*
        We must add target state to list of states unless:
        - it's already in the list;
        - it's `fin`;
        - it's `null`;
        - it's `undefined`.
        */
        static constexpr auto must_add_target_state =
            !tlu::contains_v
            <
                typename Digest::state_id_constant_list,
                constant_t<TransitionPtrConstant::value->target_state_mold>
            > &&
            !equals(TransitionPtrConstant::value->target_state_mold, state_molds::fin) &&
            !equals(TransitionPtrConstant::value->target_state_mold, null) &&
            !equals(TransitionPtrConstant::value->target_state_mold, undefined)
        ;

        using state_id_constant_list = tlu::push_back_if_t
        <
            typename Digest::state_id_constant_list,
            constant_t<TransitionPtrConstant::value->target_state_mold>,
            must_add_target_state
        >;

        static constexpr auto has_completion_transitions =
            Digest::has_completion_transitions ||
            (
                !equals(TransitionPtrConstant::value->source_state_mold, state_molds::null) &&
                is_null_v
                <
                    std::decay_t<decltype(TransitionPtrConstant::value->evt)>
                >
            )
        ;
    };

    template<const auto& TransitionTuple>
    using transition_tuple_to_ptr_constant_list =
        tuple_to_ptr_constant_list_t<TransitionTuple>
    ;
}

template<const auto& TransitionTuple>
using transition_table_digest = tlu::left_fold_t
<
    transition_table_digest_detail::transition_tuple_to_ptr_constant_list<TransitionTuple>,
    transition_table_digest_detail::add_transition_to_digest,
    transition_table_digest_detail::initial_digest
>;

} //namespace

#endif
