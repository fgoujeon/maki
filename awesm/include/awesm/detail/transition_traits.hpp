//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_TRANSITION_TRAITS_HPP
#define AWESM_DETAIL_TRANSITION_TRAITS_HPP

#include "state_traits.hpp"
#include "../transition_table.hpp"

namespace awesm::detail::transition_traits
{

template<class Region>
struct for_region
{
    template<class Transition>
    using replace_state_defs_with_states = transition
    <
        state_traits::state_def_to_state_t<typename Transition::source_state_type_pattern, Region>,
        typename Transition::event_type_pattern,
        state_traits::state_def_to_state_t<typename Transition::target_state_type, Region>,
        Transition::action,
        Transition::guard
    >;
};

} //namespace

#endif
