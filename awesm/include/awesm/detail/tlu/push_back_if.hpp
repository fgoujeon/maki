//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_TLU_PUSH_BACK_IF_HPP
#define AWESM_DETAIL_TLU_PUSH_BACK_IF_HPP

#include "push_back.hpp"

namespace awesm::detail::tlu
{

/*
push_back_if adds a type to the back of a typelist provided the given condition
is true.

In this example...:
    using typelist = tuple<char, short, int>;
    using typelist2 = push_back_if_t<typelist, long, contains_v<typelist, long>>;
    using typelist3 = push_back_if_t<typelist, short, contains_v<typelist, short>>;

... typelist2 is an alias of tuple<char, short, int, long> and
typelist3 is an alias of tuple<char, short, int>.
*/
template<class TList, class U, bool Condition>
struct push_back_if;

template<class TList, class U>
struct push_back_if<TList, U, false>
{
    using type = TList;
};

template<class TList, class U>
struct push_back_if<TList, U, true>
{
    using type = push_back_t<TList, U>;
};

template<class TList, class U, bool Condition>
using push_back_if_t = typename push_back_if<TList, U, Condition>::type;

} //namespace

#endif
