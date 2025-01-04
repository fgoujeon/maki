//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::pretty_name function template
*/

#ifndef MAKI_PRETTY_NAME_HPP
#define MAKI_PRETTY_NAME_HPP

#include "detail/type_name.hpp"

namespace maki
{

/**
@brief Gets the pretty name of a `maki::machine_conf` or `maki::state_conf`.
*/
template<const auto& Conf>
decltype(auto) pretty_name()
{
    if constexpr(opts(Conf).pretty_name.empty())
    {
        return detail::decayed_constant_name<Conf>();
    }
    else
    {
        return opts(Conf).pretty_name;
    }
}

} //namespace

#endif
