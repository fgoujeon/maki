//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_DETAIL_TUPLE_HPP
#define FGFSM_DETAIL_TUPLE_HPP

#include <tuple>

namespace fgfsm::detail
{

template<class... Ts>
using tuple = std::tuple<Ts...>;

} //namespace

#endif
