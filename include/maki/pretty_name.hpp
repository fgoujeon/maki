//Copyright Florian Goujeon 2021 - 2023.
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
@brief Gets the pretty name of a @ref machine def type, submachine type or state
type.
*/
template<class T>
decltype(auto) pretty_name()
{
    if constexpr(T::conf.pretty_name_.empty())
    {
        return detail::decayed_type_name<T>();
    }
    else
    {
        return T::conf.pretty_name_;
    }
}

template<const auto& Conf>
decltype(auto) pretty_name()
{
    if constexpr(Conf.pretty_name_.empty())
    {
        return detail::decayed_constant_name<Conf>();
    }
    else
    {
        return Conf.pretty_name_;
    }
}

} //namespace

#endif
