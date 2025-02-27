//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TRANSITION_TABLE_DIGEST_HPP
#define MAKI_DETAIL_TRANSITION_TABLE_DIGEST_HPP

#include "constant.hpp"
#include "tlu/push_back_if.hpp"
#include "tlu/left_fold.hpp"
#include "tuple.hpp"
#include "integer_constant_sequence.hpp"
#include "type_list.hpp"
#include "../null.hpp"

namespace maki::detail
{

/*
Creates a set of tuples containing all the action types, guard types and state
types of a given transition_table.

For example, the following digest type...:
    using transition_table = maki::transition_table{}
        (maki::init, state0)
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
    template<const auto& TransitionTuple>
    struct initial_digest
    {
        using state_id_constant_list = type_list_t<>;
    };

    template<const auto& TransitionTuple>
    struct add_transition_to_digest_holder
    {
        template<class Digest, int Index>
        struct add_transition_to_digest_impl
        {
            /*
            We must add target state to list of states unless:
            - it's not already in the list;
            - it's null.
            */
            static constexpr auto must_add_target_state =
                !tlu::contains_v
                <
                    typename Digest::state_id_constant_list,
                    constant_t<tuple_get<Index>(TransitionTuple).target_state_conf>
                > &&
                !equals(tuple_get<Index>(TransitionTuple).target_state_conf, null)
            ;

            using state_id_constant_list = tlu::push_back_if_t
            <
                typename Digest::state_id_constant_list,
                constant_t<tuple_get<Index>(TransitionTuple).target_state_conf>,
                must_add_target_state
            >;
        };

        template<class Digest, class IndexConstant>
        using add_transition_to_digest = add_transition_to_digest_impl
        <
            Digest,
            IndexConstant::value
        >;
    };
}

template<const auto& TransitionTuple>
using transition_table_digest = tlu::left_fold_t
<
    make_integer_constant_sequence<int, TransitionTuple.size>,
    transition_table_digest_detail::add_transition_to_digest_holder
    <
        TransitionTuple
    >::template add_transition_to_digest,
    transition_table_digest_detail::initial_digest<TransitionTuple>
>;

} //namespace

#endif
