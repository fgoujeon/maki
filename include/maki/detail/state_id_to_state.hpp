//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_ID_TO_STATE_HPP
#define MAKI_DETAIL_STATE_ID_TO_STATE_HPP

#include "state_impl.hpp"
#include "context_storage.hpp"
#include "../state.hpp"

namespace maki::detail::state_traits
{

template<const auto& MachineConf, class StateMoldPath, context_storage ParentCtxStorage>
using state_id_to_state_t = state<state_impl_t<MachineConf, StateMoldPath, ParentCtxStorage>>;

} //namespace

#endif
