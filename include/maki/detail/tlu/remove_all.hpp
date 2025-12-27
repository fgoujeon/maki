//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_REMOVE_ALL_HPP
#define MAKI_DETAIL_TLU_REMOVE_ALL_HPP

#include "filter.hpp"
#include "contains.hpp"

namespace maki::detail::tlu
{

namespace remove_all_detail
{
    template<class UList>
    struct is_not_in
    {
        template<class T>
        struct sub
        {
            static constexpr auto value = !contains_v<UList, T>;
        };
    };
}

template<class TList, class UList>
using remove_all_t = filter_t
<
    TList,
    remove_all_detail::is_not_in<UList>::template sub
>;

} //namespace

#endif
