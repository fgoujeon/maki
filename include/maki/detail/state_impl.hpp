//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_IMPL_HPP
#define MAKI_DETAIL_STATE_IMPL_HPP

#include "state_impls/simple_no_context_fwd.hpp"
#include "state_impls/simple_fwd.hpp"
#include "state_impls/composite_no_context_fwd.hpp"
#include "state_impls/composite_fwd.hpp"
#include "state_id_traits.hpp"
#include "context_storage.hpp"

namespace maki::detail::state_traits
{

template<const auto& MachineConf, class Path2, auto StateId, const auto& ParentPath, context_storage ParentCtxStorage, bool HasTransitionTables, bool HasContext>
struct state_impl_helper;

template<const auto& MachineConf, class Path2, auto StateId, const auto& ParentPath, context_storage ParentCtxStorage>
struct state_impl_helper<MachineConf, Path2, StateId, ParentPath, ParentCtxStorage, false, false>
{
    using type = state_impls::simple_no_context<StateId>;
};

template<const auto& MachineConf, class Path2, auto StateId, const auto& ParentPath, context_storage ParentCtxStorage>
struct state_impl_helper<MachineConf, Path2, StateId, ParentPath, ParentCtxStorage, false, true>
{
    using type = state_impls::simple<StateId, ParentCtxStorage>;
};

template<const auto& MachineConf, class Path2, auto StateId, const auto& ParentPath, context_storage ParentCtxStorage>
struct state_impl_helper<MachineConf, Path2, StateId, ParentPath, ParentCtxStorage, true, false>
{
    static constexpr auto path = ParentPath.template add_state<*StateId>();
    using type = state_impls::composite_no_context<MachineConf, Path2, StateId, path, ParentCtxStorage>;
};

template<const auto& MachineConf, class Path2, auto StateId, const auto& ParentPath, context_storage ParentCtxStorage>
struct state_impl_helper<MachineConf, Path2, StateId, ParentPath, ParentCtxStorage, true, true>
{
    static constexpr auto path = ParentPath.template add_state<*StateId>();
    using type = state_impls::composite<MachineConf, Path2, StateId, path, ParentCtxStorage>;
};

template<const auto& MachineConf, class Path2, auto StateId, const auto& ParentPath, context_storage ParentCtxStorage>
struct state_impl
{
    using type = typename state_impl_helper
    <
        MachineConf,
        Path2,
        StateId,
        ParentPath,
        ParentCtxStorage,
        impl_of(*StateId).transition_tables.size != 0,
        state_id_traits::has_context_v<StateId>
    >::type;
};

template<const auto& MachineConf, class Path2, auto StateId, const auto& ParentPath, context_storage ParentCtxStorage>
using state_impl_t = typename state_impl<MachineConf, Path2, StateId, ParentPath, ParentCtxStorage>::type;

} //namespace

#endif
