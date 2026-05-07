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
        return *tuple_get<Index>(impl_of(base)).target_state_mold;
    }
};

template<class Ipath, class Base>
constexpr const auto& machine_element_at_path(const Base& base)
{
    return iseq_left_fold<Ipath, machine_element_at_path_operation>(base);
}

/*
A machine element is either a `transition_table` or a `state_mold`.
An `ipath` is a path under the form of an `iseq`.
*/
template<const auto& MachineConf, class Ipath>
constexpr const auto& machine_element_at_path_v = machine_element_at_path<Ipath>(MachineConf);

} //namespace

#endif
