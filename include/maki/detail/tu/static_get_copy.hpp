//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TU_STATIC_GET_COPY_HPP
#define MAKI_DETAIL_TU_STATIC_GET_COPY_HPP

#include "get.hpp"

namespace maki::detail::tu
{

template<const auto& Tuple, int Index>
constexpr auto static_get_copy_v = get<Index>(Tuple);

} //namespace

#endif
