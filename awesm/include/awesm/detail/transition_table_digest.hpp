//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_TRANSITION_TABLE_DIGEST_HPP
#define AWESM_DETAIL_TRANSITION_TABLE_DIGEST_HPP

#include "tlu.hpp"
#include "type_list.hpp"
#include "sm_object_holder_tuple.hpp"
#include "state_wrapper.hpp"
#include "../states.hpp"
#include "../type_patterns.hpp"
#include "../transition_table.hpp"
#include "../events.hpp"
#include <type_traits>

namespace awesm::detail
{

/*
Creates a set of tuples containing all the action types, guard types and state
types of a given transition_table.

For example, the following digest type...:
    using transition_table = awesm::transition_table
    <
        awesm::row<state0, event0, state1>,
        awesm::row<state1, event1, state2, null,     guard0>,
        awesm::row<state2, event2, state3, action0>,
        awesm::row<state3, event3, state0, action1,  guard1>
    >;
    using digest = awesm::detail::transition_table_digest<transition_table>;

... is equivalent to this type:
    struct digest
    {
        using state_tuple_type = awesm::detail::type_list<awesm::states::stopped, state0, state1, state2, state3>;
        static constexpr auto has_source_state_patterns = false;
        static constexpr auto has_null_events = false;
    };
*/

namespace transition_table_digest_detail
{
    template<class RegionPath, class TList>
    struct to_tuple;

    template<class RegionPath, template<class...> class TList, class... Ts>
    struct to_tuple<RegionPath, TList<Ts...>>
    {
        using type = awesm::detail::sm_object_holder_tuple
        <
            state_wrapper_t<RegionPath, Ts>...
        >;
    };

    template<class RegionPath, class TList>
    using to_tuple_t = typename to_tuple<RegionPath, TList>::type;

    template<class TList, class U>
    using push_back_unique_if_not_null = tlu::push_back_if_t
    <
        TList,
        U,
        (!tlu::contains_v<TList, U> && !std::is_same_v<U, null>)
    >;

    template<class TransitionTable>
    using initial_state_t = typename tlu::at_t<TransitionTable, 0>::source_state_type;

    template<class InitialState>
    struct initial_digest
    {
        using state_tuple_type = type_list<states::stopped, InitialState>;
        static constexpr auto has_source_state_patterns = false;
        static constexpr auto has_null_events = false;
    };

    template<class Digest, class Row>
    struct add_row_to_digest
    {
        using state_tuple_type = push_back_unique_if_not_null
        <
            typename Digest::state_tuple_type,
            typename Row::target_state_type
        >;

        static constexpr auto has_source_state_patterns =
            Digest::has_source_state_patterns ||
            std::is_base_of_v<type_pattern, typename Row::source_state_type>
        ;

        static constexpr auto has_null_events =
            Digest::has_null_events ||
            std::is_same_v<typename Row::event_type, null>
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
        add_row_to_digest,
        initial_digest<initial_state_t<TransitionTable>>
    >;
}

template<class RegionPath, class TransitionTable>
class transition_table_digest
{
    private:
        using digest_type = transition_table_digest_detail::digest_with_type_lists
        <
            TransitionTable
        >;

    public:
        using state_tuple_type = typename digest_type::state_tuple_type;
        using wrapped_state_holder_tuple_type = transition_table_digest_detail::to_tuple_t
        <
            RegionPath,
            state_tuple_type
        >;

        static constexpr auto has_source_state_patterns = digest_type::has_source_state_patterns;
        static constexpr auto has_null_events = digest_type::has_null_events;
};

} //namespace

#endif
