//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TRANSITION_TABLE_DIGEST_HPP
#define MAKI_DETAIL_TRANSITION_TABLE_DIGEST_HPP

#include "constant.hpp"
#include "tlu/left_fold.hpp"
#include "tuple.hpp"
#include "integer_constant_sequence.hpp"
#include "type_list.hpp"
#include "../stopped.hpp"
#include "../null.hpp"
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
        using state_def_type_list = maki::detail::type_list_t<state0, state1, state2, state3>;
        static constexpr auto has_null_events = false;
    };
*/

namespace transition_table_digest_detail
{
    template<const auto& TransitionTuple>
    class initial_digest
    {
    private:
        static constexpr auto initial_state_id = tuple_get<0>(TransitionTuple).source_state_conf;

    public:
        using state_id_constant_list = type_list_t<constant_t<initial_state_id>>;
        static constexpr auto has_null_events = false;
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
            - it's null;
            - it's stopped.
            */
            static constexpr auto must_add_target_state =
                !tlu::contains_v
                <
                    typename Digest::state_id_constant_list,
                    constant_t<tuple_get<Index>(TransitionTuple).target_state_conf>
                > &&
                !equals(tuple_get<Index>(TransitionTuple).target_state_conf, null) &&
                !equals(tuple_get<Index>(TransitionTuple).target_state_conf, stopped)
            ;

            using state_id_constant_list = tlu::push_back_if_t
            <
                typename Digest::state_id_constant_list,
                constant_t<tuple_get<Index>(TransitionTuple).target_state_conf>,
                must_add_target_state
            >;

            static constexpr auto has_null_events =
                Digest::has_null_events ||
                std::is_same_v
                <
                    std::decay_t<decltype(tuple_get<Index>(TransitionTuple).evt_set)>,
                    null_t
                >
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
