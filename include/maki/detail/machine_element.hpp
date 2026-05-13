//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_MACHINE_ELEMENT_HPP
#define MAKI_DETAIL_MACHINE_ELEMENT_HPP

#include "../transition_table.hpp"
#include "../machine_conf.hpp"
#include "../state_mold.hpp"
#include "../undefined.hpp"
#include "../null.hpp"
#include "../fin.hpp"
#include "equals.hpp"
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


template<class MachineConfHolder, class TransitionTablePath, const auto& StateMold, int TransitionIndex>
constexpr int index_of_state_mold_2()
{
    constexpr const auto& target_state_mold =
        machine_element_at_path
        <
            iseq_push_back_t
            <
                TransitionTablePath,
                TransitionIndex
            >
        >(MachineConfHolder::value)
    ;

    if constexpr(ptr_equals(&StateMold, &target_state_mold))
    {
        return TransitionIndex;
    }
    else
    {
        return index_of_state_mold_2<MachineConfHolder, TransitionTablePath, StateMold, TransitionIndex + 1>();
    }
}

template<class MachineConfHolder, class TransitionTablePath, const auto& StateMold>
constexpr int index_of_state_mold()
{
    if constexpr(ptr_equals(&StateMold, &maki::undefined))
    {
        return state_mold_indexes::undefined;
    }
    else if constexpr(ptr_equals(&StateMold, &null))
    {
        return state_mold_indexes::internal;
    }
    else if constexpr(ptr_equals(&StateMold, &fin))
    {
        return state_mold_indexes::fin;
    }
    else
    {
        return index_of_state_mold_2<MachineConfHolder, TransitionTablePath, StateMold, 0>();
    }
}

template<class MachineConfHolder, class TransitionTablePath, const auto& StateMold>
constexpr int index_of_state_mold_v =
    index_of_state_mold<MachineConfHolder, TransitionTablePath, StateMold>()
;


template<class MachineConfHolder, class TransitionTablePath, int TransitionIndex, int CandidateIndex>
constexpr int index_of_source_state_mold_2()
{
    constexpr const auto& trans_table =
        machine_element_at_path<TransitionTablePath>(MachineConfHolder::value)
    ;

    constexpr const auto& source_state_mold = tuple_get<TransitionIndex>(impl_of(trans_table)).source_state_mold;

    constexpr const auto& candidate_source_state_mold =
        machine_element_at_path
        <
            iseq_push_back_t
            <
                TransitionTablePath,
                CandidateIndex
            >
        >(MachineConfHolder::value)
    ;

    if constexpr(ptr_equals(&source_state_mold, &candidate_source_state_mold))
    {
        return CandidateIndex;
    }
    else
    {
        return index_of_source_state_mold_2<MachineConfHolder, TransitionTablePath, TransitionIndex, CandidateIndex + 1>();
    }
}

template<class MachineConfHolder, class TransitionTablePath, int TransitionIndex>
constexpr int index_of_source_state_mold()
{
    constexpr const auto& trans_table =
        machine_element_at_path<TransitionTablePath>(MachineConfHolder::value)
    ;

    constexpr const auto& source_state_mold = tuple_get<TransitionIndex>(impl_of(trans_table)).source_state_mold;

    if constexpr(ptr_equals(&source_state_mold, &maki::undefined))
    {
        return state_mold_indexes::undefined;
    }
    else
    {
        return index_of_source_state_mold_2<MachineConfHolder, TransitionTablePath, TransitionIndex, 0>();
    }
}

template<class MachineConfHolder, class TransitionTablePath, int TransitionIndex>
constexpr int index_of_source_state_mold_v =
    index_of_source_state_mold<MachineConfHolder, TransitionTablePath, TransitionIndex>()
;


template<class MachineConfHolder, class TransitionTablePath, int TransitionIndex, int CandidateIndex>
constexpr int index_of_target_state_mold_2()
{
    constexpr const auto& target_state_mold =
        machine_element_at_path
        <
            iseq_push_back_t
            <
                TransitionTablePath,
                TransitionIndex
            >
        >(MachineConfHolder::value)
    ;

    constexpr const auto& candidate_target_state_mold =
        machine_element_at_path
        <
            iseq_push_back_t
            <
                TransitionTablePath,
                CandidateIndex
            >
        >(MachineConfHolder::value)
    ;

    if constexpr(ptr_equals(&target_state_mold, &candidate_target_state_mold))
    {
        return CandidateIndex;
    }
    else
    {
        return index_of_target_state_mold_2<MachineConfHolder, TransitionTablePath, TransitionIndex, CandidateIndex + 1>();
    }
}

template<class MachineConfHolder, class TransitionTablePath, int TransitionIndex>
constexpr int index_of_target_state_mold()
{
    constexpr const auto& target_state_mold =
        machine_element_at_path
        <
            iseq_push_back_t
            <
                TransitionTablePath,
                TransitionIndex
            >
        >(MachineConfHolder::value)
    ;

    if constexpr(ptr_equals(&target_state_mold, &maki::undefined))
    {
        return state_mold_indexes::undefined;
    }
    else if constexpr(ptr_equals(&target_state_mold, &null))
    {
        return state_mold_indexes::internal;
    }
    else if constexpr(ptr_equals(&target_state_mold, &fin))
    {
        return state_mold_indexes::fin;
    }
    else
    {
        return index_of_target_state_mold_2<MachineConfHolder, TransitionTablePath, TransitionIndex, 0>();
    }
}

template<class MachineConfHolder, class TransitionTablePath, int TransitionIndex>
constexpr int index_of_target_state_mold_v =
    index_of_target_state_mold<MachineConfHolder, TransitionTablePath, TransitionIndex>()
;

} //namespace

#endif
