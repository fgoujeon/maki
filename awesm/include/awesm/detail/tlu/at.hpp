//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_TLU_AT_HPP
#define AWESM_DETAIL_TLU_AT_HPP

namespace awesm::detail::tlu
{

template<int Index, class TList>
struct at;

template
<
    int Index,
    template<class...> class TList,
    class T,
    class... Ts
>
struct at<Index, TList<T, Ts...>>:
    at<Index - 1, TList<Ts...>>
{
};

template
<
    template<class...> class TList,
    class T,
    class... Ts
>
struct at<0, TList<T, Ts...>>
{
    using type = T;
};

template<class TList, int Index>
using at_t = typename at<Index, TList>::type;

} //namespace

#endif
