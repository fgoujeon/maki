//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_STATES_HPP
#define MAKI_STATES_HPP

#include "detail/state_impls/simple_no_context.hpp"
#include "state.hpp"
#include "state_builder.hpp"

namespace maki
{

namespace detail::state_builders
{
    inline constexpr auto null = state_builder{}
        .pretty_name("");

    inline constexpr auto fin = state_builder{};
}

/**
@brief The state builder of the undefined state.
*/
inline constexpr auto undefined = state_builder{};

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
    inline constexpr auto null = state<detail::state_impls::simple_no_context<&detail::state_builders::null>>{};
#endif

#if MAKI_DETAIL_DOXYGEN
    /**
    @brief Dummy final state object given to transition hooks. It represents the final state.

    Not to be confused with `maki::fin`.
    */
    constexpr auto fin = state<IMPLEMENTATION_DETAIL>{};
#else
    inline constexpr auto fin = state<detail::state_impls::simple_no_context<&detail::state_builders::fin>>{};
#endif

#if MAKI_DETAIL_DOXYGEN
    /**
    @brief Represents the undefined state.

    Not to be confused with `maki::undefined`.
    */
    constexpr auto undefined = state<IMPLEMENTATION_DETAIL>{};
#else
    inline constexpr auto undefined =
        state<detail::state_impls::simple_no_context<&maki::undefined>>{}
    ;
#endif
}

} //namespace

#endif
