//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_IPATH_UTIL_HPP
#define MAKI_DETAIL_IPATH_UTIL_HPP

#include "../state_mold.hpp"
#include "../transition_table.hpp"
#include "tuple.hpp"
#include "iseq.hpp"

namespace maki::detail
{

/*
An `ipath` is a path to a `state_mold` or a `transition_table` object under the
form of a `machine_conf` object + an `iseq`.
*/

struct ipath_to_object_operation
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
constexpr const auto& ipath_to_object(const Base& base)
{
    return iseq_left_fold<Ipath, ipath_to_object_operation>(base);
}

} //namespace

#endif
