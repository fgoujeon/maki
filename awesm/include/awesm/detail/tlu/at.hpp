//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_TLU_AT_HPP
#define AWESM_DETAIL_TLU_AT_HPP

namespace awesm::detail::tlu
{

template<class TList, int Index>
struct at;

template
<
    template<class...> class TList,
    class T,
    class... Ts,
    int Index
>
struct at<TList<T, Ts...>, Index>:
    at<TList<Ts...>, Index - 1>
{
};

template
<
    template<class...> class TList,
    class T,
    class... Ts
>
struct at<TList<T, Ts...>, 0>
{
    using type = T;
};

template<class TList, int Index>
using at_t = typename at<TList, Index>::type;

template<class TList, auto Index>
using at_f_t = at_t<TList, static_cast<int>(Index)>;

} //namespace

#endif
