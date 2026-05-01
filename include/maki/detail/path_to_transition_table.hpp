//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_PATH_TO_TRANSITION_TABLE_HPP
#define MAKI_DETAIL_PATH_TO_TRANSITION_TABLE_HPP

#include "index_sequence.hpp"

namespace maki::detail
{

template<const auto& MachineConf, class Path>
struct path_to_transition_table
{
    template<int... Indexes>
    struct helper;

    template<int TransitionTableIndex0, int StateMoldIndex, int... Indexes>
    struct helper<TransitionTableIndex0, StateMoldIndex, Indexes...>
    {
        static constexpr const auto& value =
            path_to_transition_table
            <
                *tuple_get<StateMoldIndex>(impl_of(helper<TransitionTableIndex0>::value)).target_state_mold,
                index_sequence<Indexes...>
            >::value
        ;
    };

    template<int TransitionTableIndex>
    struct helper<TransitionTableIndex>
    {
        static constexpr const auto& value =
            tuple_get<TransitionTableIndex>(impl_of(MachineConf).transition_tables)
        ;
    };

    static constexpr const auto& value =
        index_sequence_apply_t<Path, helper>::value
    ;
};

template<const auto& MachineConf, class Path>
constexpr const auto& path_to_transition_table_v =
    path_to_transition_table<MachineConf, Path>::value
;

} //namespace

#endif
