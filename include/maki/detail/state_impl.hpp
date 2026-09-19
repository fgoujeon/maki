// Copyright Florian Goujeon 2021 - 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
// Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_IMPL_HPP
#define MAKI_DETAIL_STATE_IMPL_HPP

#include "context_storage.hpp"
#include "machine_conf_tree.hpp"
#include "state_impls/forwarder_fwd.hpp"
#include "state_impls/composite_fwd.hpp"
#include "state_impls/composite_no_context_fwd.hpp"
#include "state_impls/simple_fwd.hpp"
#include "state_impls/simple_no_context_fwd.hpp"
#include <type_traits>

namespace maki::detail
{

template<
    class MachineConfHolder,
    class StateMoldPath,
    context_storage ParentCtxStorage,
    bool HasTransitionTables,
    bool IsForwarder,
    bool HasContext>
struct state_impl_helper;

template<
    class MachineConfHolder,
    class StateMoldPath,
    context_storage ParentCtxStorage>
struct state_impl_helper<
    MachineConfHolder,
    StateMoldPath,
    ParentCtxStorage,
    false,
    false,
    false>
{
    using type =
        state_impls::simple_no_context<MachineConfHolder, StateMoldPath>;
};

template<
    class MachineConfHolder,
    class StateMoldPath,
    context_storage ParentCtxStorage>
struct state_impl_helper<
    MachineConfHolder,
    StateMoldPath,
    ParentCtxStorage,
    false,
    false,
    true>
{
    using type =
        state_impls::simple<MachineConfHolder, StateMoldPath, ParentCtxStorage>;
};

template<
    class MachineConfHolder,
    class StateMoldPath,
    context_storage ParentCtxStorage>
struct state_impl_helper<
    MachineConfHolder,
    StateMoldPath,
    ParentCtxStorage,
    true,
    false,
    false>
{
    using type = state_impls::composite_no_context<
        MachineConfHolder,
        StateMoldPath,
        ParentCtxStorage>;
};

template<
    class MachineConfHolder,
    class StateMoldPath,
    context_storage ParentCtxStorage>
struct state_impl_helper<
    MachineConfHolder,
    StateMoldPath,
    ParentCtxStorage,
    true,
    false,
    true>
{
    using type = state_impls::
        composite<MachineConfHolder, StateMoldPath, ParentCtxStorage>;
};

template<
    class MachineConfHolder,
    class StateMoldPath,
    context_storage ParentCtxStorage,
    bool HasTransitionTables,
    bool HasContext>
struct state_impl_helper<
    MachineConfHolder,
    StateMoldPath,
    ParentCtxStorage,
    HasTransitionTables,
    true,
    HasContext>
{
    using type = state_impls::
        forwarder<MachineConfHolder, StateMoldPath, ParentCtxStorage>;
};

template<
    class MachineConfHolder,
    class StateMoldPath,
    context_storage ParentCtxStorage>
struct state_impl
{
    static constexpr const auto& stt_mold =
        machine_conf_tree::node_at_path_v<MachineConfHolder, StateMoldPath>;

    using context_type =
        typename std::decay_t<decltype(impl_of(stt_mold))>::context_type;

    using type = typename state_impl_helper<
        MachineConfHolder,
        StateMoldPath,
        ParentCtxStorage,
        is_composite(impl_of(stt_mold)),
        is_forwarder(impl_of(stt_mold)),
        !std::is_void_v<context_type>>::type;
};

template<
    class MachineConfHolder,
    class StateMoldPath,
    context_storage ParentCtxStorage>
using state_impl_t =
    typename state_impl<MachineConfHolder, StateMoldPath, ParentCtxStorage>::
        type;

} // namespace maki::detail

#endif
