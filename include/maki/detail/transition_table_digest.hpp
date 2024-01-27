//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TRANSITION_TABLE_DIGEST_HPP
#define MAKI_DETAIL_TRANSITION_TABLE_DIGEST_HPP

#include "../type_patterns.hpp"
#include "../transition_table.hpp"
#include "../events.hpp"
#include "constant.hpp"
#include "same_ref.hpp"
#include "tlu.hpp"
#include "tuple.hpp"
#include "tu.hpp"
#include "state_traits.hpp"
#include "type_list.hpp"
#include <type_traits>

namespace maki::detail
{

/*
Creates a set of tuples containing all the action types, guard types and state
types of a given transition_table.

For example, the following digest type...:
    using transition_table = maki::transition_table
    <
        maki::transition<state0, event0, state1>,
        maki::transition<state1, event1, state2, null,   guard0>,
        maki::transition<state2, event2, state3, action0>,
        maki::transition<state3, event3, state0, action1,  guard1>
    >;
    using digest = maki::detail::transition_table_digest<transition_table>;

... is equivalent to this type:
    struct digest
    {
        using state_def_type_list = maki::type_list<state0, state1, state2, state3>;
        static constexpr auto has_null_events = false;
    };
*/

template<class StateConfPtrTuple>
struct transition_table_digest_t
{
    StateConfPtrTuple state_conf_ptrs;
    bool has_null_events = false;
};

template<class StateConfPtrTuple>
constexpr auto make_transition_table_digest(const StateConfPtrTuple& state_conf_ptrs, bool has_null_events)
{
    return transition_table_digest_t<StateConfPtrTuple>{state_conf_ptrs, has_null_events};
}

namespace transition_table_digest_detail
{
    template<const auto& TransitionTuple>
    constexpr auto initial_digest = make_transition_table_digest
    (
        tu::append(tuple{}, tu::static_get_copy_v<TransitionTuple, 0>.psource_state_conf_pattern),
        false
    );

    template<const auto& Digest, const auto& Transition>
    struct add_transition_to_digest
    {
        static constexpr auto state_conf_ptrs = []
        {
            if constexpr
            (
                static_cast<const void*>(Transition.ptarget_state_conf) != static_cast<const void*>(&null) &&
                !tu::contains(Digest.state_conf_ptrs, Transition.ptarget_state_conf)
            )
            {
                return tu::append
                (
                    Digest.state_conf_ptrs,
                    Transition.ptarget_state_conf
                );
            }
            else
            {
                return Digest.state_conf_ptrs;
            }
        }();

        static constexpr auto has_null_events =
            Digest.has_null_events ||
            is_null(Transition.event_pattern)
        ;

        static constexpr auto value = make_transition_table_digest
        (
            state_conf_ptrs,
            has_null_events
        );
    };
}

template<const auto& TransitionTuple>
constexpr auto transition_table_digest = tu::static_left_fold_v
<
    TransitionTuple,
    transition_table_digest_detail::add_transition_to_digest,
    transition_table_digest_detail::initial_digest<TransitionTuple>
>;

} //namespace

#endif
