//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_MOLD_INDEXES_HPP
#define MAKI_DETAIL_STATE_MOLD_INDEXES_HPP

namespace maki::detail::state_mold_ids
{

/*
Given to indicate that a transition is internal (i.e. `null` target).
*/
inline constexpr auto internal = -4;

/*
Given either as:
- source to indicate that a composite state must enter some substate without
  exiting any state;
- target to indicate that a composite state must exit its active substate
  without entering any state.
*/
inline constexpr auto null = -3;

/*
The ID of the undefined `state_mold`.
*/
inline constexpr auto undefined = -2;

/*
The ID of the final `state_mold`.
*/
inline constexpr auto fin = -1;

/*
The ID of the initial `state_mold`, as it's always the target of the first
transition in a transition table.
*/
inline constexpr auto ini = 0;

} //namespace

#endif
