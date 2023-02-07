//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_SM_TRAITS_HPP
#define AWESM_DETAIL_SM_TRAITS_HPP

#include "../transition_table.hpp"
#include "clu.hpp"
#include <type_traits>

namespace awesm::detail::sm_traits
{

template<class Sm>
using transition_table_fn_list_t = to_transition_table_fn_list_t<Sm::conf::transition_table_fn>;

template<class Sm>
inline constexpr auto region_count_v = clu::size_v<transition_table_fn_list_t<Sm>>;

} //namespace

#endif
