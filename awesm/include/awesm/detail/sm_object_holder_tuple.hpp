//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_SM_OBJECT_HOLDER_TUPLE_HPP
#define AWESM_DETAIL_SM_OBJECT_HOLDER_TUPLE_HPP

#include "tuple.hpp"
#include "sm_object_holder.hpp"

namespace awesm::detail
{

template<class... Ts>
using sm_object_holder_tuple_t = tuple<sm_object_holder<Ts>...>;

} //namespace

#endif
