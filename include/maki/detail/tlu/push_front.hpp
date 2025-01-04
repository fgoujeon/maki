//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_PUSH_FRONT_HPP
#define MAKI_DETAIL_TLU_PUSH_FRONT_HPP

namespace maki::detail::tlu
{

/*
push_front adds a type to the front of a typelist.

In this example...:
    using typelist = tuple<char, short, int>;
    using typelist2 = push_front_t<typelist, long>;

... typelist2 is an alias of tuple<char, short, int, long>.
*/
template<class TListT, class U>
struct push_front;

template<class U, template<class...> class TList, class... Ts>
struct push_front<TList<Ts...>, U>
{
    using type = TList<U, Ts...>;
};

template<class TListT, class U>
using push_front_t = typename push_front<TListT, U>::type;

} //namespace

#endif
