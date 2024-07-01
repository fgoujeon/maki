//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_CONF_TRAITS_HPP
#define MAKI_DETAIL_CONF_TRAITS_HPP

#include "../null.hpp"
#include "../state_conf_fwd.hpp"
#include "../submachine_conf_fwd.hpp"
#include <type_traits>

namespace maki::detail::conf_traits
{

template<class T>
struct is_submachine_conf
{
    static constexpr auto value = false;
};

template<class... Ts>
struct is_submachine_conf<submachine_conf<Ts...>>
{
    static constexpr auto value = true;
};

template<class T>
constexpr auto is_submachine_conf_v = is_submachine_conf<T>::value;

} //namespace

#endif
