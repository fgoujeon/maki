//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TRANSITION_TABLE_DIGEST_HPP
#define MAKI_DETAIL_TRANSITION_TABLE_DIGEST_HPP

#include "tuple.hpp"
#include "machine_conf_tree.hpp"
#include "iseq.hpp"
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
        using stt_mold_ids = maki::detail::iseq<0, 1, 2, 3>;
    };
*/

namespace transition_table_digest_detail
{
    struct initial_digest
    {
        using stt_mold_ids = iseq<>;
        static constexpr auto has_completion_transitions = false;
    };

    template<class MachineConfHolder, class TransitionTablePath>
    struct add_transition_to_digest_holder
    {
        template<class Digest, int TransitionIndex>
        struct add_transition_to_digest
        {
            static constexpr int target_state_mold_id = machine_conf_tree::id_of_target_state_mold_v
            <
                MachineConfHolder,
                TransitionTablePath,
                TransitionIndex
            >;

            /*
            We must add target state to list of states unless:
            - it's already in the list;
            - it's `fin`;
            - it's `null`;
            - it's `undefined`.
            */
            static constexpr auto must_add_target_state =
                target_state_mold_id == TransitionIndex
            ;

            using stt_mold_ids =
                iseq_push_back_if_t
                <
                    typename Digest::stt_mold_ids,
                    TransitionIndex,
                    must_add_target_state
                >
            ;

            static constexpr auto has_completion_transitions =
                Digest::has_completion_transitions ||
                (
                    TransitionIndex != 0 &&
                    is_null_v
                    <
                        std::decay_t
                        <
                            decltype
                            (
                                tuple_get<TransitionIndex>
                                (
                                    impl_of
                                    (
                                        machine_conf_tree::node_at_path_v<MachineConfHolder, TransitionTablePath>
                                    )
                                ).evt
                            )
                        >
                    >
                )
            ;
        };
    };
}

template<class MachineConfHolder, class TransitionTablePath>
using transition_table_digest = iseq_left_fold_t
<
    linear_iseq_t
    <
        impl_of
        (
            machine_conf_tree::node_at_path_v<MachineConfHolder, TransitionTablePath>
        ).size
    >,
    transition_table_digest_detail::add_transition_to_digest_holder
    <
        MachineConfHolder,
        TransitionTablePath
    >::template add_transition_to_digest,
    transition_table_digest_detail::initial_digest
>;

} //namespace

#endif
