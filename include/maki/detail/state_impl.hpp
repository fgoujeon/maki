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
#include "machine_element.hpp"
#include "context_storage.hpp"

namespace maki::detail::state_traits
{

template<class MachineConfHolder, class StateMoldPath, context_storage ParentCtxStorage, bool HasTransitionTables, bool HasContext>
struct state_impl_helper;

template<class MachineConfHolder, class StateMoldPath, context_storage ParentCtxStorage>
struct state_impl_helper<MachineConfHolder, StateMoldPath, ParentCtxStorage, false, false>
{
    using type = state_impls::simple_no_context<&machine_element_at_path<StateMoldPath>(MachineConfHolder::value)>;
};

template<class MachineConfHolder, class StateMoldPath, context_storage ParentCtxStorage>
struct state_impl_helper<MachineConfHolder, StateMoldPath, ParentCtxStorage, false, true>
{
    using type = state_impls::simple<&machine_element_at_path<StateMoldPath>(MachineConfHolder::value), ParentCtxStorage>;
};

template<class MachineConfHolder, class StateMoldPath, context_storage ParentCtxStorage>
struct state_impl_helper<MachineConfHolder, StateMoldPath, ParentCtxStorage, true, false>
{
    using type = state_impls::composite_no_context<MachineConfHolder, StateMoldPath, ParentCtxStorage>;
};

template<class MachineConfHolder, class StateMoldPath, context_storage ParentCtxStorage>
struct state_impl_helper<MachineConfHolder, StateMoldPath, ParentCtxStorage, true, true>
{
    using type = state_impls::composite<MachineConfHolder, StateMoldPath, ParentCtxStorage>;
};

template<class MachineConfHolder, class StateMoldPath, context_storage ParentCtxStorage>
struct state_impl
{
    static constexpr auto state_id = &machine_element_at_path<StateMoldPath>(MachineConfHolder::value);
    using type = typename state_impl_helper
    <
        MachineConfHolder,
        StateMoldPath,
        ParentCtxStorage,
        impl_of(*state_id).transition_tables.size != 0,
        state_id_traits::has_context_v<state_id>
    >::type;
};

template<class MachineConfHolder, class StateMoldPath, context_storage ParentCtxStorage>
using state_impl_t = typename state_impl<MachineConfHolder, StateMoldPath, ParentCtxStorage>::type;

} //namespace

#endif
