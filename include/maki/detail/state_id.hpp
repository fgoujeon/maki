//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_ID_HPP
#define MAKI_DETAIL_STATE_ID_HPP

#include "../state_conf_fwd.hpp"
#include "../machine_conf_fwd.hpp"

namespace maki::detail
{

/*
The ID of a state is just the address of its configuration.
*/

template<class T>
struct to_state_id_or_identity
{
    using type = T;
};

template<class OptionSet>
struct to_state_id_or_identity<state_conf<OptionSet>>
{
    using type = const state_conf<OptionSet>*;
};

template<class OptionSet>
struct to_state_id_or_identity<machine_conf<OptionSet>>
{
    using type = const machine_conf<OptionSet>*;
};

template<class T>
using to_state_id_or_identity_t = typename to_state_id_or_identity<T>::type;

template<class T>
constexpr decltype(auto) try_making_state_id(T&& obj)
{
    return std::forward<T>(obj);
}

template<class OptionSet>
constexpr auto try_making_state_id(const state_conf<OptionSet>& conf)
{
    return &conf;
}

template<class OptionSet>
constexpr auto try_making_state_id(const machine_conf<OptionSet>& conf)
{
    return &conf;
}

} //namespace

#endif
