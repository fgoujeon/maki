//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_GET_HPP
#define MAKI_DETAIL_TLU_GET_HPP

namespace maki::detail::tlu
{

template<class TList, int Index>
struct get;

template
<
    template<class...> class TList,
    class T,
    class... Ts,
    int Index
>
struct get<TList<T, Ts...>, Index>:
    get<TList<Ts...>, Index - 1>
{
};

template
<
    template<class...> class TList,
    class T,
    class... Ts
>
struct get<TList<T, Ts...>, 0>
{
    using type = T;
};

template<class TList, int Index>
using get_t = typename get<TList, Index>::type;

} //namespace

#endif
