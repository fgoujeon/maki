//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_PUSH_FRONT_IF_HPP
#define MAKI_DETAIL_TLU_PUSH_FRONT_IF_HPP

#include "push_front.hpp"

namespace maki::detail::tlu
{

template<class TList, class U, bool Condition>
struct push_front_if;

template<class TList, class U>
struct push_front_if<TList, U, false>
{
    using type = TList;
};

template<class TList, class U>
struct push_front_if<TList, U, true>
{
    using type = push_front_t<TList, U>;
};

template<class TList, class U, bool Condition>
using push_front_if_t = typename push_front_if<TList, U, Condition>::type;

} //namespace

#endif
