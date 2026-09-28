// Copyright Florian Goujeon 2021 - 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
// Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TRANSITION_TABLE_TRAITS_HPP
#define MAKI_DETAIL_TRANSITION_TABLE_TRAITS_HPP

#include "../null.hpp"
#include "iseq.hpp"
#include "machine_conf_tree.hpp"
#include "tuple.hpp"
#include <type_traits>

namespace maki::detail::transition_table_traits
{

/*
state_mold_ids
*/

namespace state_mold_ids_detail
{
    template<class MachineConfHolder, class TransitionTablePath>
    struct predicate
    {
        template<int TransitionIndex>
        struct inner
        {
            static constexpr int target_state_mold_id =
                machine_conf_tree::id_of_target_state_mold_v<
                    MachineConfHolder,
                    TransitionTablePath,
                    TransitionIndex>;

            /*
            We must add target state to list of states unless:
            - it's already in the list;
            - it's `fin`;
            - it's `null`;
            - it's `undefined`.
            */
            static constexpr bool value =
                target_state_mold_id == TransitionIndex;
        };
    };
} // namespace state_mold_ids_detail

/*
Creates a sequence of transition indexes. The target state molds of these
transitions are all the state molds referred to by the transition table.

For example, the following sequence...:
    using transition_table = maki::transition_table{}
        (maki::ini,  state0)
        (state0,     state1, event0)
        (state1,     state2, event1, null,    guard0)
        (state2,     state3, event2, action0)
        (state3,     state0, event3, action1, guard1)
    >;
    using sequence = state_mold_ids<MachineConfHolder,
path_to_transition_table>;

... is equivalent to this type:
    maki::detail::iseq<0, 1, 2, 3>;
*/
template<class MachineConfHolder, class TransitionTablePath>
using state_mold_ids = iseq_filter_t<
    linear_iseq_t<impl_of(
        machine_conf_tree::
            node_at_path_v<MachineConfHolder, TransitionTablePath>)
            .size>,
    state_mold_ids_detail::predicate<MachineConfHolder, TransitionTablePath>::
        template inner>;


/*
has_completion_transitions
*/

namespace has_completion_transitions_detail
{
    template<class MachineConfHolder, class TransitionTablePath>
    struct predicate
    {
        template<int TransitionIndex>
        struct inner
        {
            static constexpr bool value =
                (TransitionIndex != 0 &&
                    is_null_v<std::decay_t<decltype(tuple_get<TransitionIndex>(
                        impl_of(
                            machine_conf_tree::node_at_path_v<
                                MachineConfHolder,
                                TransitionTablePath>))
                            .evt)>>);
        };
    };
} // namespace has_completion_transitions_detail

template<class MachineConfHolder, class TransitionTablePath>
static constexpr bool has_completion_transitions = iseq_contains_if_v<
    linear_iseq_t<impl_of(
        machine_conf_tree::
            node_at_path_v<MachineConfHolder, TransitionTablePath>)
            .size>,
    has_completion_transitions_detail::
        predicate<MachineConfHolder, TransitionTablePath>::template inner>;

} // namespace maki::detail::transition_table_traits

#endif
