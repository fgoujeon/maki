//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_REMOVE_HPP
#define MAKI_DETAIL_TLU_REMOVE_HPP

#include "filter.hpp"
#include <type_traits>

namespace maki::detail::tlu
{

namespace remove_detail
{
    template<class U>
    struct is_not
    {
        template<class T>
        struct sub
        {
            static constexpr auto value = !std::is_same_v<T, U>;
        };
    };
}

template<class TList, class U>
using remove_t = filter_t
<
    TList,
    remove_detail::is_not<U>::template sub
>;

} //namespace

#endif
