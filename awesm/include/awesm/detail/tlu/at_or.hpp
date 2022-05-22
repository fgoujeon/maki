//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_TLU_AT_OR_HPP
#define AWESM_DETAIL_TLU_AT_OR_HPP

namespace awesm::detail::tlu
{

namespace at_or_detail
{
    template<int Index, class Default, class TList>
    struct at;

    template
    <
        int Index,
        class Default,
        template<class...> class TList,
        class T,
        class... Ts
    >
    struct at<Index, Default, TList<T, Ts...>>:
        at<Index - 1, Default, TList<Ts...>>
    {
    };

    template
    <
        class Default,
        template<class...> class TList,
        class T,
        class... Ts
    >
    struct at<0, Default, TList<T, Ts...>>
    {
        using type = T;
    };

    template<int Index, class Default, template<class...> class TList>
    struct at<Index, Default, TList<>>
    {
        using type = Default;
    };
}

template<class TList, int Index, class Default>
using at_or = typename at_or_detail::at<Index, Default, TList>::type;

} //namespace

#endif
