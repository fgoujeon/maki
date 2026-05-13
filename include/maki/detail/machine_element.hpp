//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_MACHINE_ELEMENT_HPP
#define MAKI_DETAIL_MACHINE_ELEMENT_HPP

#include "../state_mold.hpp"
#include "../transition_table.hpp"
#include "../machine_conf.hpp"
#include "../undefined.hpp"
#include "state_molds.hpp"
#include "state_mold_indexes.hpp"
#include "tuple.hpp"
#include "iseq.hpp"

namespace maki::detail
{

struct machine_element_at_path_operation
{
    template<int Index, class MachineConfImpl>
    static constexpr const auto& call(const machine_conf<MachineConfImpl>& base)
    {
        return tuple_get<Index>(impl_of(base).transition_tables);
    }

    template<int Index, class StateMoldImpl>
    static constexpr const auto& call(const state_mold<StateMoldImpl>& base)
    {
        return tuple_get<Index>(impl_of(base).transition_tables);
    }

    template<int Index, class TransitionTableImpl>
    static constexpr const auto& call(const transition_table<TransitionTableImpl>& base)
    {
        if constexpr(Index == state_mold_indexes::null)
        {
            return state_molds::null;
        }
        else if constexpr(Index == state_mold_indexes::undefined)
        {
            return undefined;
        }
        else if constexpr(Index == state_mold_indexes::fin)
        {
            return state_molds::fin;
        }
        else
        {
            return tuple_get<Index>(impl_of(base)).target_state_mold;
        }
    }
};

template<class Ipath, class Base>
constexpr const auto& machine_element_at_path(const Base& base)
{
    return iseq_left_fold<Ipath, machine_element_at_path_operation>(base);
}

} //namespace

#endif
