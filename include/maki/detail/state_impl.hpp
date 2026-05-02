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
#include "ipath_util.hpp"
#include "context_storage.hpp"

namespace maki::detail::state_traits
{

template<const auto& MachineConf, class StateMoldPath, const auto& ParentPath, context_storage ParentCtxStorage, bool HasTransitionTables, bool HasContext>
struct state_impl_helper;

template<const auto& MachineConf, class StateMoldPath, const auto& ParentPath, context_storage ParentCtxStorage>
struct state_impl_helper<MachineConf, StateMoldPath, ParentPath, ParentCtxStorage, false, false>
{
    using type = state_impls::simple_no_context<&ipath_to_state_mold_v<MachineConf, StateMoldPath>>;
};

template<const auto& MachineConf, class StateMoldPath, const auto& ParentPath, context_storage ParentCtxStorage>
struct state_impl_helper<MachineConf, StateMoldPath, ParentPath, ParentCtxStorage, false, true>
{
    using type = state_impls::simple<&ipath_to_state_mold_v<MachineConf, StateMoldPath>, ParentCtxStorage>;
};

template<const auto& MachineConf, class StateMoldPath, const auto& ParentPath, context_storage ParentCtxStorage>
struct state_impl_helper<MachineConf, StateMoldPath, ParentPath, ParentCtxStorage, true, false>
{
    static constexpr auto state_id = &ipath_to_state_mold_v<MachineConf, StateMoldPath>;
    static constexpr auto path = ParentPath.template add_state<*state_id>();
    using type = state_impls::composite_no_context<MachineConf, StateMoldPath, path, ParentCtxStorage>;
};

template<const auto& MachineConf, class StateMoldPath, const auto& ParentPath, context_storage ParentCtxStorage>
struct state_impl_helper<MachineConf, StateMoldPath, ParentPath, ParentCtxStorage, true, true>
{
    static constexpr auto state_id = &ipath_to_state_mold_v<MachineConf, StateMoldPath>;
    static constexpr auto path = ParentPath.template add_state<*state_id>();
    using type = state_impls::composite<MachineConf, StateMoldPath, path, ParentCtxStorage>;
};

template<const auto& MachineConf, class StateMoldPath, const auto& ParentPath, context_storage ParentCtxStorage>
struct state_impl
{
    static constexpr auto state_id = &ipath_to_state_mold_v<MachineConf, StateMoldPath>;
    using type = typename state_impl_helper
    <
        MachineConf,
        StateMoldPath,
        ParentPath,
        ParentCtxStorage,
        impl_of(*state_id).transition_tables.size != 0,
        state_id_traits::has_context_v<state_id>
    >::type;
};

template<const auto& MachineConf, class StateMoldPath, const auto& ParentPath, context_storage ParentCtxStorage>
using state_impl_t = typename state_impl<MachineConf, StateMoldPath, ParentPath, ParentCtxStorage>::type;

} //namespace

#endif
