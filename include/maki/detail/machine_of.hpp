//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_MACHINE_OF_HPP
#define MAKI_DETAIL_MACHINE_OF_HPP

#include "submachine_fwd.hpp"
#include "simple_state_fwd.hpp"
#include "region_fwd.hpp"
#include "../machine_fwd.hpp"

namespace maki::detail
{

template<class T>
struct machine_of;

template<class T>
using machine_of_t = typename machine_of<T>::type;

template<class ConfHolder>
struct machine_of<machine<ConfHolder>>
{
    using type = machine<ConfHolder>;
};

template<const auto& Conf, class Parent>
struct machine_of<submachine<Conf, Parent>>
{
    using type = machine_of_t<Parent>;
};

template<const auto& Conf, class Parent>
struct machine_of<simple_state<Conf, Parent>>
{
    using type = machine_of_t<Parent>;
};

template<class ParentSm, int Index>
struct machine_of<region<ParentSm, Index>>
{
    using type = machine_of_t<ParentSm>;
};

} //namespace

#endif
