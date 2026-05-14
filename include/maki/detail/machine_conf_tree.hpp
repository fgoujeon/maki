//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_MACHINE_CONF_TREE_HPP
#define MAKI_DETAIL_MACHINE_CONF_TREE_HPP

#include "../transition_table.hpp"
#include "../machine_conf.hpp"
#include "../state_mold.hpp"
#include "../undefined.hpp"
#include "../null.hpp"
#include "../fin.hpp"
#include "equals.hpp"
#include "state_molds.hpp"
#include "state_mold_ids.hpp"
#include "tuple.hpp"
#include "iseq.hpp"

/*
A `machine_conf` can be seen as the root of a tree:
- a `machine_conf` has a list of `transition_table`s;
- a `transition_table` has a list of `state_mold`s;
- a `state_mold` has a list of `transition_table`s;
and so on.

What's interesting about it is that, at every level, every child has a locally
unique integer ID that can be easily resolved.

Every `transition_table` is stored in a tuple, which is indexed.

A `state_mold` can be associated with the index of the first transition (of its
parent `transition_table`) where it appears as a target. For example, in the
following transition table, the ID of `off` is 0, and the ID of `on` is 1:

    ```
    (ini, off)
    (off, on,  power_button_press)
    (on,  off, power_button_press)
    ```

Thanks to this, any node of a `machine_conf` tree can be accessed with an
`iseq`. For example, `iseq<0, 3, 1>` refers to the 2nd (ID 1) `transition_table`
of the target `state_mold` of the 4th (ID 3) transition of the 1st (ID 0)
`transition_table` of the machine.

Note: To avoid confusion with `maki::path`, a path under the form of an `iseq`
is called an `ipath`.

All of this allows our templates to take two types (a `MachineConfHolder` and
an `iseq` path) instead of a reference to a node (i.e. a `transition_table` or
a `state_mold`), which significantly improves build time for most compilers.
*/

namespace maki::detail::machine_conf_tree
{

/*
`node_at_path`
*/

struct node_at_path_operation
{
    template<int Id, class MachineConfImpl>
    static constexpr const auto& call(const machine_conf<MachineConfImpl>& base)
    {
        return tuple_get<Id>(impl_of(base).transition_tables);
    }

    template<int Id, class StateMoldImpl>
    static constexpr const auto& call(const state_mold<StateMoldImpl>& base)
    {
        return tuple_get<Id>(impl_of(base).transition_tables);
    }

    template<int Id, class TransitionTableImpl>
    static constexpr const auto& call(const transition_table<TransitionTableImpl>& base)
    {
        if constexpr(Id == state_mold_ids::null)
        {
            return state_molds::null;
        }
        else if constexpr(Id == state_mold_ids::undefined)
        {
            return undefined;
        }
        else if constexpr(Id == state_mold_ids::fin)
        {
            return state_molds::fin;
        }
        else
        {
            return tuple_get<Id>(impl_of(base)).target_state_mold;
        }
    }
};

template<class Ipath, class MachineConf>
constexpr const auto& node_at_path(const MachineConf& mach_conf)
{
    return iseq_left_fold<Ipath, node_at_path_operation>(mach_conf);
}

template<class MachineConfHolder, class Ipath>
constexpr const auto& node_at_path_v =
    node_at_path<Ipath>(MachineConfHolder::value)
;


/*
`id_of_state_mold_v`
*/

template<class MachineConfHolder, class TransitionTablePath, const auto& StateMold, int TransitionIndex>
constexpr int id_of_state_mold_2()
{
    constexpr const auto& target_state_mold =
        machine_conf_tree::node_at_path_v
        <
            MachineConfHolder,
            iseq_push_back_t
            <
                TransitionTablePath,
                TransitionIndex
            >
        >
    ;

    if constexpr(ptr_equals(&StateMold, &target_state_mold))
    {
        return TransitionIndex;
    }
    else
    {
        return id_of_state_mold_2<MachineConfHolder, TransitionTablePath, StateMold, TransitionIndex + 1>();
    }
}

template<class MachineConfHolder, class TransitionTablePath, const auto& StateMold>
constexpr int id_of_state_mold()
{
    if constexpr(ptr_equals(&StateMold, &maki::undefined))
    {
        return state_mold_ids::undefined;
    }
    else if constexpr(ptr_equals(&StateMold, &null))
    {
        return state_mold_ids::internal;
    }
    else if constexpr(ptr_equals(&StateMold, &fin))
    {
        return state_mold_ids::fin;
    }
    else
    {
        return id_of_state_mold_2<MachineConfHolder, TransitionTablePath, StateMold, 0>();
    }
}

/*
The ID of `StateMold` in the `transition_table` at `TransitionTablePath`.
*/
template<class MachineConfHolder, class TransitionTablePath, const auto& StateMold>
constexpr int id_of_state_mold_v =
    id_of_state_mold<MachineConfHolder, TransitionTablePath, StateMold>()
;


/*
`id_of_source_state_mold_v`
*/

template<class MachineConfHolder, class TransitionTablePath, int TransitionIndex, int CandidateIndex>
constexpr int id_of_source_state_mold_2()
{
    constexpr const auto& trans_table =
        machine_conf_tree::node_at_path_v<MachineConfHolder, TransitionTablePath>
    ;

    constexpr const auto& source_state_mold = tuple_get<TransitionIndex>(impl_of(trans_table)).source_state_mold;

    constexpr const auto& candidate_source_state_mold =
        machine_conf_tree::node_at_path_v
        <
            MachineConfHolder,
            iseq_push_back_t
            <
                TransitionTablePath,
                CandidateIndex
            >
        >
    ;

    if constexpr(ptr_equals(&source_state_mold, &candidate_source_state_mold))
    {
        return CandidateIndex;
    }
    else
    {
        return id_of_source_state_mold_2<MachineConfHolder, TransitionTablePath, TransitionIndex, CandidateIndex + 1>();
    }
}

template<class MachineConfHolder, class TransitionTablePath, int TransitionIndex>
constexpr int id_of_source_state_mold()
{
    constexpr const auto& trans_table =
        machine_conf_tree::node_at_path_v<MachineConfHolder, TransitionTablePath>
    ;

    constexpr const auto& source_state_mold = tuple_get<TransitionIndex>(impl_of(trans_table)).source_state_mold;

    if constexpr(ptr_equals(&source_state_mold, &maki::undefined))
    {
        return state_mold_ids::undefined;
    }
    else
    {
        return id_of_source_state_mold_2<MachineConfHolder, TransitionTablePath, TransitionIndex, 0>();
    }
}

/*
The ID of the source `state_mold` of the transition at `TransitionIndex` of the
`transition_table` at `TransitionTablePath`.
*/
template<class MachineConfHolder, class TransitionTablePath, int TransitionIndex>
constexpr int id_of_source_state_mold_v =
    id_of_source_state_mold<MachineConfHolder, TransitionTablePath, TransitionIndex>()
;


/*
`id_of_target_state_mold_v`
*/

template<class MachineConfHolder, class TransitionTablePath, int TransitionIndex, int CandidateIndex>
constexpr int id_of_target_state_mold_2()
{
    constexpr const auto& target_state_mold =
        machine_conf_tree::node_at_path_v
        <
            MachineConfHolder,
            iseq_push_back_t
            <
                TransitionTablePath,
                TransitionIndex
            >
        >
    ;

    constexpr const auto& candidate_target_state_mold =
        machine_conf_tree::node_at_path_v
        <
            MachineConfHolder,
            iseq_push_back_t
            <
                TransitionTablePath,
                CandidateIndex
            >
        >
    ;

    if constexpr(ptr_equals(&target_state_mold, &candidate_target_state_mold))
    {
        return CandidateIndex;
    }
    else
    {
        return id_of_target_state_mold_2<MachineConfHolder, TransitionTablePath, TransitionIndex, CandidateIndex + 1>();
    }
}

template<class MachineConfHolder, class TransitionTablePath, int TransitionIndex>
constexpr int id_of_target_state_mold()
{
    constexpr const auto& target_state_mold =
        machine_conf_tree::node_at_path_v
        <
            MachineConfHolder,
            iseq_push_back_t
            <
                TransitionTablePath,
                TransitionIndex
            >
        >
    ;

    if constexpr(ptr_equals(&target_state_mold, &maki::undefined))
    {
        return state_mold_ids::undefined;
    }
    else if constexpr(ptr_equals(&target_state_mold, &null))
    {
        return state_mold_ids::internal;
    }
    else if constexpr(ptr_equals(&target_state_mold, &fin))
    {
        return state_mold_ids::fin;
    }
    else
    {
        return id_of_target_state_mold_2<MachineConfHolder, TransitionTablePath, TransitionIndex, 0>();
    }
}

/*
The ID of the target `state_mold` of the transition at `TransitionIndex` of the
`transition_table` at `TransitionTablePath`.
*/
template<class MachineConfHolder, class TransitionTablePath, int TransitionIndex>
constexpr int id_of_target_state_mold_v =
    id_of_target_state_mold<MachineConfHolder, TransitionTablePath, TransitionIndex>()
;

} //namespace

#endif
