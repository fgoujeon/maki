//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_SIMPLE_STATE_FWD_HPP
#define MAKI_DETAIL_SIMPLE_STATE_FWD_HPP

#include "state_id_traits.hpp"

namespace maki::detail
{

template<auto Id, class Context>
class simple_state_impl;

template<auto Id>
using simple_state = simple_state_impl<Id, state_id_traits::context_t<Id>>;

} //namespace

#endif
