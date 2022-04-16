//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_DETAIL_TLU_PUSH_BACK_UNIQUE_HPP
#define FGFSM_DETAIL_TLU_PUSH_BACK_UNIQUE_HPP

#include "contains.hpp"
#include "push_back_if.hpp"

namespace fgfsm::detail::tlu
{

/*
push_back_unique adds a type to the back of a typelist provided it's not already
in the typelist.

In this example...:
    using typelist = tuple<char, short, int>;
    using typelist2 = push_back_unique<typelist, long>;
    using typelist3 = push_back_unique<typelist, short>;

... typelist2 is an alias of tuple<char, short, int, long> and
typelist3 is an alias of tuple<char, short, int>.
*/

template<class TList, class U>
using push_back_unique = push_back_if<TList, U, !contains<TList, U>>;

} //namespace

#endif
