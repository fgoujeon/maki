//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_MACHINE_OBJECT_HOLDER_TUPLE_HPP
#define MAKI_DETAIL_MACHINE_OBJECT_HOLDER_TUPLE_HPP

#include "tuple.hpp"
#include "machine_object_holder.hpp"

namespace maki::detail
{

template<class... Ts>
using machine_object_holder_tuple_t = tuple<machine_object_holder<Ts>...>;

} //namespace

#endif
