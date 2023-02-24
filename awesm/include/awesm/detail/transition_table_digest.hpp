//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_TRANSITION_TABLE_DIGEST_HPP
#define AWESM_DETAIL_TRANSITION_TABLE_DIGEST_HPP

#include "tlu.hpp"
#include "type_list.hpp"
#include "tuple.hpp"
#include "sm_object_holder.hpp"
#include "state_traits.hpp"
#include "../type_patterns.hpp"
#include "../transition_table.hpp"
#include "../events.hpp"
#include "../null.hpp"
#include <type_traits>

namespace awesm::detail
{

/*
Creates a set of tuples containing all the action types, guard types and state
types of a given transition_table.

For example, the following digest type...:
    using transition_table = awesm::transition_table
    <
        awesm::transition<state0, event0, state1>,
        awesm::transition<state1, event1, state2, null,     guard0>,
        awesm::transition<state2, event2, state3, action0>,
        awesm::transition<state3, event3, state0, action1,  guard1>
    >;
    using digest = awesm::detail::transition_table_digest<transition_table>;

... is equivalent to this type:
    struct digest
    {
        using state_tuple_type = awesm::detail::type_list<state0, state1, state2, state3>;
        static constexpr auto has_null_events = false;
    };
*/

namespace transition_table_digest_detail
{
    template<class Region>
    struct state_wrapper_tuple_holder
    {
        template<class... Ts>
        using type = tuple
        <
            sm_object_holder
            <
                state_traits::wrap_t<Ts, Region>
            >...
        >;
    };

    template<class TList, class U>
    using push_back_unique_if_not_null = tlu::push_back_if_t
    <
        TList,
        U,
        (!tlu::contains_v<TList, U> && !std::is_same_v<U, null>)
    >;

    template<class TransitionTable>
    using initial_state_t = typename tlu::at_t<TransitionTable, 0>::source_state_type_pattern;

    template<class InitialState>
    struct initial_digest
    {
        using state_tuple_type = type_list<InitialState>;
        static constexpr auto has_null_events = false;
    };

    template<class Digest, class Transition>
    struct add_transition_to_digest
    {
        using state_tuple_type = push_back_unique_if_not_null
        <
            typename Digest::state_tuple_type,
            typename Transition::target_state_type
        >;

        static constexpr auto has_null_events =
            Digest::has_null_events ||
            std::is_same_v<typename Transition::event_type_pattern, null>
        ;
    };

    /*
    First step with type_list instead of awesm::detail::sm_object_holder_tuple,
    so that we don't instantiate intermediate tuples.
    */
    template<class TransitionTable>
    using digest_with_type_lists = tlu::left_fold_t
    <
        TransitionTable,
        add_transition_to_digest,
        initial_digest<initial_state_t<TransitionTable>>
    >;
}

template<class TransitionTable, class Region>
class transition_table_digest
{
    private:
        using digest_type = transition_table_digest_detail::digest_with_type_lists
        <
            TransitionTable
        >;

    public:
        using state_tuple_type = typename digest_type::state_tuple_type;
        using wrapped_state_holder_tuple_type = tlu::apply_t
        <
            state_tuple_type,
            transition_table_digest_detail::state_wrapper_tuple_holder<Region>::template type
        >;

        static constexpr auto has_null_events = digest_type::has_null_events;
};

} //namespace

#endif
