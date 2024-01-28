//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_TRAITS_HPP
#define MAKI_DETAIL_STATE_TRAITS_HPP

#include "submachine_fwd.hpp"
#include "simple_state_fwd.hpp"
#include "tlu.hpp"
#include "same_ref.hpp"
#include "../null.hpp"
#include "../type_patterns.hpp"
#include "../submachine_conf.hpp"
#include <type_traits>

namespace maki::detail::state_traits
{

//state_conf_to_state

template<const auto& StateConf, class Parent, class Enable = void>
struct state_conf_to_state
{
    using type = simple_state<StateConf>;
};

template<const auto& StateConf, class Parent>
struct state_conf_to_state<StateConf, Parent, std::enable_if_t<is_submachine_conf_v<std::decay_t<decltype(StateConf)>>>>
{
    using type = submachine<StateConf, Parent>;
};

template<const auto& StateConf, class Parent>
using state_conf_to_state_t = typename state_conf_to_state<StateConf, Parent>::type;


//has_conf

template<const auto& Conf>
struct for_conf
{
    template<class T>
    struct has_conf
    {
        static constexpr auto value = same_ref(T::conf, Conf);
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
