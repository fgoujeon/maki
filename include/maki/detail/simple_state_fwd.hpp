//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_SIMPLE_STATE_FWD_HPP
#define MAKI_DETAIL_SIMPLE_STATE_FWD_HPP

#include "conf_traits.hpp"

namespace maki::detail
{

template<const auto& Conf, class Context>
class simple_state_impl;

template<const auto& Conf>
using simple_state = simple_state_impl<Conf, conf_traits::context_t<Conf>>;

} //namespace

#endif
