//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_IMPL_HPP
#define MAKI_DETAIL_STATE_IMPL_HPP

#include "state_impls/simple_fwd.hpp"
#include "state_id_traits.hpp"
#include "context_storage.hpp"

namespace maki::detail::state_traits
{

template<auto StateId, const auto& ParentPath, context_storage ParentCtxStorage>
using state_impl_t = state_impls::simple
<
    StateId,
    state_id_traits::context_t<StateId>,
    ParentCtxStorage
>;

} //namespace

#endif
