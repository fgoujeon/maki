//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_FLATTEN_TRANSITION_TABLE_HPP
#define MAKI_DETAIL_FLATTEN_TRANSITION_TABLE_HPP

#include "../null.hpp"
#include "../transition_table.hpp"
#include "tuple.hpp"
#include "friendly_impl.hpp"
#include <type_traits>

namespace maki::detail
{

template<class TransitionTableImpl, class InitialFlatTransitionTuple>
constexpr auto flatten_transition_table_2
(
    const transition_table<TransitionTableImpl>& table,
    const InitialFlatTransitionTuple& initial_flat
);

namespace flatten_transition_table_detail
{
    template<class Transition>
    constexpr bool has_composite_target_state()
    {
        using target_state_mold_type =
            std::decay_t<std::remove_pointer_t<typename Transition::target_state_mold_type>>
        ;

        if constexpr(std::is_same_v<target_state_mold_type, null_t_impl>)
        {
            return false;
        }
        else
        {
            using target_state_mold_impl_type =
                impl_of_t<target_state_mold_type>
            ;

            using target_state_transition_table_tuple_type =
                typename target_state_mold_impl_type::transition_table_tuple_type
            ;

            return target_state_transition_table_tuple_type::size > 0;
        }
    }

    template<class Transition>
    constexpr bool has_composite_target_state_v = has_composite_target_state<Transition>();

    template
    <
        class StateMold,
        class InitialStateSubstitudeMold
    >
    constexpr auto substitute_initial_pseudostate
    (
        const StateMold* pmold,
        const InitialStateSubstitudeMold& initial_state_substitute_mold
    )
    {
        if constexpr(std::is_same_v<typename StateMold::context_type, null_context>)
        {
            return initial_state_substitute_mold;
        }
        else
        {
            return pmold;
        }
    }

    template
    <
        class StateMold,
        class FinalStateSubstitudeMold
    >
    constexpr auto substitute_final_state
    (
        const StateMold& mold,
        const FinalStateSubstitudeMold& final_state_substitute_mold
    )
    {
        using state_mold_type =
            std::decay_t<std::remove_pointer_t<StateMold>>
        ;

        if constexpr(std::is_same_v<state_mold_type, null_t_impl>)
        {
            return mold;
        }
        else
        {
            using state_mold_impl_type =
                impl_of_t<state_mold_type>
            ;

            using state_context_type =
                typename state_mold_impl_type::context_type
            ;

            if constexpr(std::is_same_v<state_context_type, fin_context>)
            {
                return final_state_substitute_mold;
            }
            else
            {
                return mold;
            }
        }
    }

    template
    <
        class InitialStateSubstitudeMold,
        class FinalStateSubstitudeMold
    >
    struct operation
    {
        template<class Flat, class Transition>
        constexpr auto operator()(const Flat& flat, const Transition& trans) const
        {
            if constexpr(has_composite_target_state_v<Transition>)
            {
                return flatten_transition_table_2
                (
                    tuple_get<0>(impl_of(*trans.target_state_mold).transition_tables),
                    flat,
                    substitute_initial_pseudostate(trans.source_state_mold, initial_state_substitute_mold),
                    substitute_final_state(trans.target_state_mold, final_state_substitute_mold)
                );
            }
            else
            {
                return flat.append
                (
                    detail::transition
                    {
                        substitute_initial_pseudostate(trans.source_state_mold, initial_state_substitute_mold),
                        substitute_final_state(trans.target_state_mold, final_state_substitute_mold),
                        trans.evt,
                        trans.act,
                        trans.grd
                    }
                );
            }
        }

        InitialStateSubstitudeMold initial_state_substitute_mold;
        FinalStateSubstitudeMold final_state_substitute_mold;
    };
}

template
<
    class TransitionTableImpl,
    class InitialFlatTransitionTuple,
    class InitialStateSubstitudeMold,
    class FinalStateSubstitudeMold
>
constexpr auto flatten_transition_table_2
(
    const transition_table<TransitionTableImpl>& table,
    const InitialFlatTransitionTuple& initial_flat,
    const InitialStateSubstitudeMold& initial_state_substitute_mold,
    const FinalStateSubstitudeMold& final_state_substitute_mold
)
{
    return tuple_left_fold
    (
        impl_of(table),
        flatten_transition_table_detail::operation
        <
            InitialStateSubstitudeMold,
            FinalStateSubstitudeMold
        >
        {
            initial_state_substitute_mold,
            final_state_substitute_mold
        },
        initial_flat
    );
}

template<class TransitionTableImpl>
constexpr auto flatten_transition_table(const transition_table<TransitionTableImpl>& table)
{
    return make_transition_table
    (
        flatten_transition_table_2(table, make_tuple(), store_state_mold(ini), store_state_mold(fin))
    );
}

} //namespace

#endif
