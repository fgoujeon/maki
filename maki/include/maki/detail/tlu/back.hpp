//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_BACK_HPP
#define MAKI_DETAIL_TLU_BACK_HPP

#include "get.hpp"
#include "size.hpp"

namespace maki::detail::tlu
{

template<class TList>
using back_t = get_t<TList, size_v<TList> - 1>;

} //namespace

#endif
