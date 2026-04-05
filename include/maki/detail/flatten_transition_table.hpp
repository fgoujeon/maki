//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_FLATTEN_TRANSITION_TABLE_HPP
#define MAKI_DETAIL_FLATTEN_TRANSITION_TABLE_HPP

#include "tuple.hpp"

namespace maki::detail
{

namespace flatten_transition_table_detail
{
    struct operation_t
    {
        template<class Flat, class Transition>
        constexpr auto operator()(const Flat& flat, const Transition& trans) const
        {
            return flat.append(trans);
        }
    };

    inline constexpr operation_t operation;
}

template<class TransitionTable>
constexpr auto flatten_transition_table(const TransitionTable& table)
{
    return make_transition_table
    (
        tuple_left_fold
        (
            impl_of(table),
            flatten_transition_table_detail::operation,
            make_tuple()
        )
    );
}

} //namespace

#endif
