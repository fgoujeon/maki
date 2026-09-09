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
#include "machine_conf_tree.hpp"
#include "context_storage.hpp"
#include <type_traits>

#ifdef MAKI_DETAIL_STATE_IMPL_HPP_2
#include "aos_async_begin.hpp"
#else
#include "aos_sync_begin.hpp"
#endif

namespace maki::detail
{

template<class MachineConfHolder, class StateMoldPath, context_storage ParentCtxStorage, bool HasTransitionTables, bool HasContext>
struct MAKI_AOS_NAME(state_impl_helper);

template<class MachineConfHolder, class StateMoldPath, context_storage ParentCtxStorage>
struct MAKI_AOS_NAME(state_impl_helper)<MachineConfHolder, StateMoldPath, ParentCtxStorage, false, false>
{
    using type = state_impls::MAKI_AOS_NAME(simple_no_context)<MachineConfHolder, StateMoldPath>;
};

template<class MachineConfHolder, class StateMoldPath, context_storage ParentCtxStorage>
struct MAKI_AOS_NAME(state_impl_helper)<MachineConfHolder, StateMoldPath, ParentCtxStorage, false, true>
{
    using type = state_impls::MAKI_AOS_NAME(simple)<MachineConfHolder, StateMoldPath, ParentCtxStorage>;
};

template<class MachineConfHolder, class StateMoldPath, context_storage ParentCtxStorage>
struct MAKI_AOS_NAME(state_impl_helper)<MachineConfHolder, StateMoldPath, ParentCtxStorage, true, false>
{
    using type = state_impls::MAKI_AOS_NAME(composite_no_context)<MachineConfHolder, StateMoldPath, ParentCtxStorage>;
};

template<class MachineConfHolder, class StateMoldPath, context_storage ParentCtxStorage>
struct MAKI_AOS_NAME(state_impl_helper)<MachineConfHolder, StateMoldPath, ParentCtxStorage, true, true>
{
    using type = state_impls::MAKI_AOS_NAME(composite)<MachineConfHolder, StateMoldPath, ParentCtxStorage>;
};

template<class MachineConfHolder, class StateMoldPath, context_storage ParentCtxStorage>
struct MAKI_AOS_NAME(state_impl)
{
    static constexpr const auto& stt_mold = machine_conf_tree::node_at_path_v<MachineConfHolder, StateMoldPath>;

    using context_type = typename std::decay_t<decltype(impl_of(stt_mold))>::context_type;

    using type = typename MAKI_AOS_NAME(state_impl_helper)
    <
        MachineConfHolder,
        StateMoldPath,
        ParentCtxStorage,
        impl_of(stt_mold).transition_tables.size != 0,
        !std::is_void_v<context_type>
    >::type;
};

template<class MachineConfHolder, class StateMoldPath, context_storage ParentCtxStorage>
using MAKI_AOS_NAME(state_impl_t) = typename MAKI_AOS_NAME(state_impl)
<
    MachineConfHolder,
    StateMoldPath,
    ParentCtxStorage
>::type;

} //namespace

#include "aos_end.hpp"

// Reinclude a second time
#ifndef MAKI_DETAIL_STATE_IMPL_HPP_2
#define MAKI_DETAIL_STATE_IMPL_HPP_2
#undef MAKI_DETAIL_STATE_IMPL_HPP
#include "state_impl.hpp"
#endif

#endif
