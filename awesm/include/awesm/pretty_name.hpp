//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_PRETTY_NAME_HPP
#define AWESM_PRETTY_NAME_HPP

#include "detail/type_name.hpp"
#include <string_view>

namespace awesm
{

template<class T>
constexpr auto pretty_name = detail::type_name<T>;

} //namespace

#endif
