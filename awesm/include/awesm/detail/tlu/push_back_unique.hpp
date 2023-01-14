//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_TLU_PUSH_BACK_UNIQUE_HPP
#define AWESM_DETAIL_TLU_PUSH_BACK_UNIQUE_HPP

#include "contains.hpp"
#include "push_back_if.hpp"

namespace awesm::detail::tlu
{

/*
push_back_unique_t adds a type to the back of a typelist provided it's not already
in the typelist.

In this example...:
    using typelist = tuple<char, short, int>;
    using typelist2 = push_back_unique_t<typelist, long>;
    using typelist3 = push_back_unique_t<typelist, short>;

... typelist2 is an alias of tuple<char, short, int, long> and
typelist3 is an alias of tuple<char, short, int>.
*/
template<class TList, class U>
using push_back_unique_t = push_back_if_t<TList, U, !contains_v<TList, U>>;

} //namespace

#endif
