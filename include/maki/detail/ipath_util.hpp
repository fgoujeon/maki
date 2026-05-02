//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_IPATH_UTIL_HPP
#define MAKI_DETAIL_IPATH_UTIL_HPP

#include "tuple.hpp"
#include "index_sequence.hpp"

namespace maki::detail
{

/*
An `ipath` is a path to a `state_mold` or a `transition_table` object under the
form of a `machine_conf` object + an `index_sequence`.
*/

namespace ipath_util_detail
{
    /*
    Template declarations
    */

    template<const auto& StateMold, int... Indexes>
    struct state_mold_and_ints_to_transition_table;

    template<const auto& StateMold, int... Indexes>
    struct state_mold_and_ints_to_state_mold;

    template<const auto& TransitionTable, int StateMoldIndex, int... Indexes>
    struct transition_table_and_ints_to_state_mold
    {
        static constexpr const auto& value =
            state_mold_and_ints_to_state_mold
            <
                *tuple_get<StateMoldIndex>(impl_of(TransitionTable)).target_state_mold,
                Indexes...
            >::value
        ;
    };


    /*
    `state_mold_and_ints_to_transition_table` specializations
    */

    template<const auto& StateMold, int TransitionTableIndex, int StateMoldIndex, int... Indexes>
    struct state_mold_and_ints_to_transition_table<StateMold, TransitionTableIndex, StateMoldIndex, Indexes...>
    {
        static constexpr const auto& value =
            state_mold_and_ints_to_transition_table
            <
                state_mold_and_ints_to_state_mold
                <
                    StateMold,
                    TransitionTableIndex,
                    StateMoldIndex
                >::value,
                Indexes...
            >::value
        ;
    };

    template<const auto& StateMold, int TransitionTableIndex>
    struct state_mold_and_ints_to_transition_table<StateMold, TransitionTableIndex>
    {
        static constexpr auto value =
            tuple_get<TransitionTableIndex>(impl_of(StateMold).transition_tables)
        ;
    };


    /*
    `state_mold_and_ints_to_state_mold` specializations
    */

    template<const auto& StateMold, int TransitionTableIndex, int... Indexes>
    struct state_mold_and_ints_to_state_mold<StateMold, TransitionTableIndex, Indexes...>
    {
        static constexpr const auto& value =
            transition_table_and_ints_to_state_mold
            <
                state_mold_and_ints_to_transition_table
                <
                    StateMold,
                    TransitionTableIndex
                >::value,
                Indexes...
            >::value
        ;
    };

    template<const auto& StateMold>
    struct state_mold_and_ints_to_state_mold<StateMold>
    {
        static constexpr const auto& value = StateMold;
    };


    /*
    `with_machine_conf`
    */

    template<const auto& MachineConf>
    struct with_machine_conf
    {
        template<int... Indexes>
        using ints_to_transition_table = state_mold_and_ints_to_transition_table<MachineConf, Indexes...>;

        template<int... Indexes>
        using ints_to_state_mold = state_mold_and_ints_to_state_mold<MachineConf, Indexes...>;
    };
}

template<const auto& MachineConf, class Path>
constexpr const auto& ipath_to_transition_table_v =
    index_sequence_apply_t<Path, ipath_util_detail::with_machine_conf<MachineConf>::template ints_to_transition_table>::value
;

template<const auto& MachineConf, class Path>
constexpr const auto& ipath_to_state_mold_v =
    index_sequence_apply_t<Path, ipath_util_detail::with_machine_conf<MachineConf>::template ints_to_state_mold>::value
;

} //namespace

#endif
