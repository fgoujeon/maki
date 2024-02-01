//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_CONTEXT_OF_HPP
#define MAKI_DETAIL_CONTEXT_OF_HPP

#include "simple_state_fwd.hpp"
#include "submachine_fwd.hpp"
#include "region_fwd.hpp"
#include "../machine_fwd.hpp"
#include "../path.hpp"

namespace maki::detail
{

template<class T>
struct context_of;

template<class T>
using context_of_t = typename context_of<T>::type;

template<class T>
using decayed_context_of_t = std::decay_t<context_of_t<T>>;

template<class ConfHolder>
struct context_of<machine<ConfHolder>>
{
    using type = typename std::decay_t<decltype(opts(ConfHolder::conf))>::context_type;
};

template<const auto& Conf, class Parent>
struct context_of<submachine<Conf, Parent>>
{
    using own_context_type = typename std::decay_t<decltype(opts(Conf))>::context_type;

    using type = std::conditional_t
    <
        std::is_same_v<own_context_type, null_t>,
        decayed_context_of_t<Parent>&,
        own_context_type
    >;
};

template<class ParentSm, int Index>
struct context_of<region<ParentSm, Index>>
{
    using type = decayed_context_of_t<ParentSm>&;
};

template<const auto& Conf, class DecayedParentContext>
struct context_of<simple_state<Conf, DecayedParentContext>>
{
    using own_context_type = typename std::decay_t<decltype(opts(Conf))>::context_type;

    using type = std::conditional_t
    <
        std::is_same_v<own_context_type, null_t>,
        DecayedParentContext&,
        own_context_type
    >;
};

} //namespace

#endif
