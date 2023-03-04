//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_CLU_AT_HPP
#define AWESM_DETAIL_CLU_AT_HPP

namespace awesm::detail::clu
{

template<class CList, int Index>
struct at;

template
<
    template<auto...> class CList,
    auto C,
    auto... Cs,
    int Index
>
struct at<CList<C, Cs...>, Index>:
    at<CList<Cs...>, Index - 1>
{
};

template
<
    template<auto...> class CList,
    auto C,
    auto... Cs
>
struct at<CList<C, Cs...>, 0>
{
    static constexpr auto value = C;
};

template<class CList, int Index>
inline constexpr auto at_v = at<CList, Index>::value;

} //namespace

#endif
