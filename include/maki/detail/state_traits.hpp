//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_TRAITS_HPP
#define MAKI_DETAIL_STATE_TRAITS_HPP

#include "simple_state_no_context_fwd.hpp"
#include "simple_state_fwd.hpp"
#include "submachine_no_context_fwd.hpp"
#include "submachine_fwd.hpp"
#include "conf_traits.hpp"
#include "state_id_traits.hpp"
#include "tlu.hpp"
#include "same_ref.hpp"
#include "../null.hpp"
#include "../filters.hpp"
#include "../submachine_conf.hpp"
#include <type_traits>

namespace maki::detail::state_traits
{

//state_id_to_state

template<auto StateId, class Parent, bool IsSubmachine, bool HasContext>
struct state_id_to_state_impl;

template<auto StateId, class Parent>
struct state_id_to_state_impl<StateId, Parent, false, false>
{
    using type = simple_state_no_context<StateId>;
};

template<auto StateId, class Parent>
struct state_id_to_state_impl<StateId, Parent, false, true>
{
    using type = simple_state<StateId>;
};

template<auto StateId, class Parent>
struct state_id_to_state_impl<StateId, Parent, true, false>
{
    using type = submachine_no_context<StateId, Parent>;
};

template<auto StateId, class Parent>
struct state_id_to_state_impl<StateId, Parent, true, true>
{
    using type = submachine<StateId, Parent>;
};

template<auto StateId, class Parent>
struct state_id_to_state
{
    using type = typename state_id_to_state_impl
    <
        StateId,
        Parent,
        conf_traits::is_submachine_conf_v<std::decay_t<decltype(*StateId)>>,
        state_id_traits::has_context_v<StateId>
    >::type;
};

template<auto StateId, class Parent>
using state_id_to_state_t = typename state_id_to_state
<
    StateId,
    Parent
>::type;


//has_id

template<auto Id>
struct for_id
{
    template<class T>
    struct has_id
    {
        static constexpr auto value = static_cast<const void*>(T::identifier) == static_cast<const void*>(Id);
    };
};

} //namespace

#endif
