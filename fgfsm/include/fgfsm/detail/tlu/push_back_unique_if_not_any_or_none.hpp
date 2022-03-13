//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_DETAIL_TLU_PUSH_BACK_UNIQUE_IF_NOT_ANY_OR_NONE_HPP
#define FGFSM_DETAIL_TLU_PUSH_BACK_UNIQUE_IF_NOT_ANY_OR_NONE_HPP

#include "contains.hpp"
#include "push_back_if.hpp"
#include <fgfsm/any.hpp>
#include <fgfsm/none.hpp>
#include <type_traits>

namespace fgfsm::detail::tlu
{

template<class TList, class U>
using push_back_unique_if_not_any_or_none = push_back_if
<
    TList,
    U,
    !contains<TList, U> && !std::is_same_v<U, any> && !std::is_same_v<U, none>
>;

} //namespace

#endif
