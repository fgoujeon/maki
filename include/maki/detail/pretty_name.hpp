//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::pretty_name function template
*/

#ifndef MAKI_DETAIL_PRETTY_NAME_HPP
#define MAKI_DETAIL_PRETTY_NAME_HPP

#include "type_name.hpp"

namespace maki::detail
{

/**
@brief Gets the pretty name of `maki::state_mold`.
*/
template<const auto& Mold>
decltype(auto) pretty_name()
{
    if constexpr(impl_of(Mold).pretty_name.data() == nullptr)
    {
        return detail::decayed_constant_name<Mold>();
    }
    else
    {
        return impl_of(Mold).pretty_name;
    }
}

} //namespace

#endif
