//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_SUBMACHINE_FWD_HPP
#define MAKI_DETAIL_SUBMACHINE_FWD_HPP

#include "conf_traits.hpp"
#include <type_traits>

namespace maki::detail
{

template<auto Id, class ParentRegion, class Context>
class submachine_impl;

template<auto Id, class ParentRegion>
using submachine = submachine_impl<Id, ParentRegion, conf_traits::context_t<*Id>>;

} //namespace

#endif
