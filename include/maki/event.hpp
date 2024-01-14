//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_EVENT_HPP
#define MAKI_EVENT_HPP

#include "type.hpp"

namespace maki
{

template<class Event>
constexpr auto event = type<Event>{};

} //namespace

#endif
