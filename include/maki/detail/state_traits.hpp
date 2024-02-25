//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_TRAITS_HPP
#define MAKI_DETAIL_STATE_TRAITS_HPP

#include "conf_traits.hpp"
#include "submachine_fwd.hpp"
#include "simple_state_fwd.hpp"
#include "tlu.hpp"
#include "same_ref.hpp"
#include "../null.hpp"
#include "../type_filters.hpp"
#include "../submachine_conf.hpp"
#include <type_traits>

namespace maki::detail::state_traits
{

//state_id_to_state

template<auto StateId, class Parent, class Enable = void>
struct state_id_to_state
{
    using type = simple_state<StateId>;
};

template<auto StateId, class Parent>
struct state_id_to_state<StateId, Parent, std::enable_if_t<conf_traits::is_submachine_conf_v<std::decay_t<decltype(*StateId)>>>>
{
    using type = submachine<StateId, Parent>;
};

template<auto StateId, class Parent>
using state_id_to_state_t = typename state_id_to_state<StateId, Parent>::type;


//has_id

template<auto Id>
struct for_id
{
    template<class T>
    struct has_id
    {
        static constexpr auto value = static_cast<const void*>(T::id) == static_cast<const void*>(Id);
    };
};


//has_conf_ptr

template<auto ConfPtr>
struct for_conf_ptr
{
    template<class T>
    struct has_conf_ptr
    {
        static constexpr auto value = &T::conf == static_cast<const void*>(ConfPtr);
    };
};

} //namespace

#endif
