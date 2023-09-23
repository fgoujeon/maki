//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TRANSITION_TABLE_DIGEST_HPP
#define MAKI_DETAIL_TRANSITION_TABLE_DIGEST_HPP

#include "tlu.hpp"
#include "tuple.hpp"
#include "tuple_2.hpp"
#include "machine_object_holder.hpp"
#include "state_traits.hpp"
#include "type_list.hpp"
#include "../type_patterns.hpp"
#include "../transition_table.hpp"
#include "../events.hpp"
#include <type_traits>

namespace maki::detail
{

template<class TransitionTable>
struct transition_table_digest;

template<class Transition, class... Transitions>
struct transition_table_digest<transition_table_tpl<Transition, Transitions...>>
{
    static constexpr auto states = make_tuple_2(&Transition::source_state, &Transition::target_state, &Transitions::target_state...);
};

} //namespace

#endif
