//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_PUSH_BACK_ALL_UNIQUE_HPP
#define MAKI_DETAIL_TLU_PUSH_BACK_ALL_UNIQUE_HPP

#include "left_fold.hpp"
#include "push_back_unique.hpp"

namespace maki::detail::tlu
{

/*
`push_back_all_unique_t` adds every type from `UList` to the back of `TList`
provided it's not already in `TList`.
*/
template<class TList, class UList>
using push_back_all_unique_t = left_fold_t
<
    UList,
    push_back_unique_t,
    TList
>;

} //namespace

#endif
