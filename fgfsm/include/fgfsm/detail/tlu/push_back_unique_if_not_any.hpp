//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_DETAIL_TLU_PUSH_BACK_UNIQUE_IF_NOT_ANY_HPP
#define FGFSM_DETAIL_TLU_PUSH_BACK_UNIQUE_IF_NOT_ANY_HPP

#include "contains.hpp"
#include "push_back_unique.hpp"
#include <fgfsm/any.hpp>
#include <type_traits>

namespace fgfsm::detail::tlu
{

/*
push_back_unique_if_not_any adds a type to the back of a typelist provided:
- it's not already in the typelist;
- it's not fgfsm::any.

In this example...:
    using typelist = std::tuple<char, short, int>;
    using typelist2 = push_back_unique<typelist, long>;
    using typelist3 = push_back_unique<typelist, short>;
    using typelist4 = push_back_unique<typelist, fgfsm::any>;

- typelist2 is an alias of std::tuple<char, short, int, long>;
- typelist3 is an alias of std::tuple<char, short, int>;
- typelist4 is an alias of std::tuple<char, short, int>.
*/

namespace push_back_unique_if_not_any_detail
{
    template<class TList, class U, bool Contained>
    struct push_back;

    template<class TList, class U>
    struct push_back<TList, U, true>
    {
        using type = TList;
    };

    template<class TList, class U>
    struct push_back<TList, U, false>
    {
        using type = push_back_unique<TList, U>;
    };
}

template<class TList, class U>
using push_back_unique_if_not_any =
    typename push_back_unique_if_not_any_detail::push_back
    <
        TList,
        U,
        std::is_same_v<U, any>
    >::type
;

} //namespace

#endif
