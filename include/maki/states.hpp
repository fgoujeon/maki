//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_STATES_HPP
#define MAKI_STATES_HPP

#include "machine_conf.hpp"
#include "transition_table.hpp"
#include "state.hpp"
#include "detail/state_mold_ids.hpp"
#include "detail/state_impls/simple_no_context.hpp"
#include "detail/iseq.hpp"

namespace maki
{

namespace detail
{
    inline constexpr auto dummy_machine_transition_table = transition_table{};

    inline constexpr auto dummy_machine_transition_table_index = 0;

    struct dummy_machine_conf_holder
    {
        static constexpr auto value = machine_conf{}
            .transition_tables(dummy_machine_transition_table)
        ;
    };
}

/**
@brief Predefined state and pseudostate objects.
*/
namespace states
{
#if MAKI_DETAIL_DOXYGEN
    /**
    @brief Dummy state object given to transition hooks.

    It represents either:

    - the source state for transitions from the initial pseudostate;
    - the target state for transitions that exits the superstate.

    Its pretty name is an empty string.

    Not to be confused with `maki::null`.
    */
    constexpr auto null = state<IMPLEMENTATION_DETAIL>{};
#else
    inline constexpr auto null = state
    <
        detail::state_impls::simple_no_context
        <
            detail::dummy_machine_conf_holder,
            detail::iseq
            <
                detail::dummy_machine_transition_table_index,
                detail::state_mold_ids::null
            >
        >
    >{};
#endif

#if MAKI_DETAIL_DOXYGEN
    /**
    @brief Dummy final state object given to transition hooks. It represents the final state.

    Not to be confused with `maki::fin`.
    */
    constexpr auto fin = state<IMPLEMENTATION_DETAIL>{};
#else
    inline constexpr auto fin = state
    <
        detail::state_impls::simple_no_context
        <
            detail::dummy_machine_conf_holder,
            detail::iseq
            <
                detail::dummy_machine_transition_table_index,
                detail::state_mold_ids::fin
            >
        >
    >{};
#endif

#if MAKI_DETAIL_DOXYGEN
    /**
    @brief Represents the undefined state.

    Not to be confused with `maki::undefined`.
    */
    constexpr auto undefined = state<IMPLEMENTATION_DETAIL>{};
#else
    inline constexpr auto undefined = state
    <
        detail::state_impls::simple_no_context
        <
            detail::dummy_machine_conf_holder,
            detail::iseq
            <
                detail::dummy_machine_transition_table_index,
                detail::state_mold_ids::undefined
            >
        >
    >{};
#endif
}

} //namespace

#endif
