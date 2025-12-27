//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_INTERSECTION_HPP
#define MAKI_DETAIL_TLU_INTERSECTION_HPP

#include "filter.hpp"
#include "contains.hpp"

namespace maki::detail::tlu
{

namespace intersection_detail
{
    template<class UList>
    struct is_in
    {
        template<class T>
        struct sub
        {
            static constexpr auto value = contains_v<UList, T>;
        };
    };
}

template<class TList, class UList>
using intersection_t = filter_t
<
    TList,
    intersection_detail::is_in<UList>::template sub
>;

} //namespace

#endif
