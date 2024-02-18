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

namespace transition_table_digest_detail
{
    template<class TList, auto ConfPtr>
    using push_back_unique_if_not_null_constant = tlu::push_back_if_t
    <
        TList,
        constant_t<ConfPtr>,
        (
            !tlu::contains_v<TList, constant_t<ConfPtr>> &&
            static_cast<const void*>(ConfPtr) != static_cast<const void*>(&null)
        )
    >;

    template<class TransitionPtrConstantList>
    class initial_digest
    {
    private:
        static constexpr auto pinitial_state_conf = tlu::get_t<TransitionPtrConstantList, 0>::value->source_state_conf_pattern.get_as_ptr();

    public:
        using state_conf_ptr_constant_list = type_list<constant_t<pinitial_state_conf>>;
        static constexpr auto has_null_events = false;
    };

    template<class Digest, class TransitionPtrConstant>
    struct add_transition_to_digest
    {
        using state_conf_ptr_constant_list = push_back_unique_if_not_null_constant
        <
            typename Digest::state_conf_ptr_constant_list,
            TransitionPtrConstant::value->target_state_conf.get_as_ptr()
        >;

        static constexpr auto has_null_events =
            Digest::has_null_events ||
            is_null(TransitionPtrConstant::value->event_pattern)
        ;
    };
}

template<class TransitionPtrConstantList>
using transition_table_digest = tlu::left_fold_t
<
    TransitionPtrConstantList,
    transition_table_digest_detail::add_transition_to_digest,
    transition_table_digest_detail::initial_digest<TransitionPtrConstantList>
>;

} //namespace

#endif
