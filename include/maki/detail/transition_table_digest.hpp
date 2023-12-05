//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TRANSITION_TABLE_DIGEST_HPP
#define MAKI_DETAIL_TRANSITION_TABLE_DIGEST_HPP

#include "../type_list.hpp"
#include "../type_patterns.hpp"
#include "../transition_table.hpp"
#include "../events.hpp"
#include "same_ref.hpp"
#include "tlu.hpp"
#include "tuple.hpp"
#include "machine_object_holder.hpp"
#include "state_traits.hpp"
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
        maki::transition<state1, event1, state2, null,     guard0>,
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
    template<class Region>
    struct state_conf_constant_list_to_state_type_list_holder
    {
        template<class... ConfConstants>
        using type = type_list<state_traits::state_conf_to_state_t<ConfConstants::value, Region>...>;
    };

    template<class TList, const auto& Conf>
    using push_back_unique_if_not_null_constant = tlu::push_back_if_t
    <
        TList,
        constant<Conf>,
        (
            !tlu::contains_v<TList, constant<Conf>> &&
            !same_ref(Conf, null)
        )
    >;

    template<class TransitionTable>
    constexpr const auto& initial_state_conf_c = tlu::get_t<TransitionTable, 0>::source_state_conf_pattern;

    template<const auto& InitialStateConf>
    struct initial_digest
    {
        using state_conf_constant_list = type_list<constant<InitialStateConf>>;
        static constexpr auto has_null_events = false;
    };

    template<class Digest, class Transition>
    struct add_transition_to_digest
    {
        using state_conf_constant_list = push_back_unique_if_not_null_constant
        <
            typename Digest::state_conf_constant_list,
            Transition::target_state_conf
        >;

        static constexpr auto has_null_events =
            Digest::has_null_events ||
            std::is_same_v<typename Transition::event_type_pattern, null_t>
        ;
    };

    /*
    First step with tlu::type_list instead of
    maki::detail::machine_object_holder_tuple, so that we don't instantiate
    intermediate tuples.
    */
    template<class TransitionTable>
    using digest_with_type_lists = tlu::left_fold_t
    <
        TransitionTable,
        add_transition_to_digest,
        initial_digest<initial_state_conf_c<TransitionTable>>
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
    using state_conf_constant_list = typename digest_type::state_conf_constant_list;
    using state_type_list = tlu::apply_t
    <
        state_conf_constant_list,
        transition_table_digest_detail::state_conf_constant_list_to_state_type_list_holder<Region>::template type
    >;

    static constexpr auto has_null_events = digest_type::has_null_events;
};

} //namespace

#endif
