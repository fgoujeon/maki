//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_EQUALS_HPP
#define MAKI_DETAIL_EQUALS_HPP

#include <type_traits>

namespace maki::detail
{

template<class Lhs, class Rhs>
constexpr bool equals(const Lhs& lhs, const Rhs& rhs)
{
    if constexpr(std::is_same_v<Lhs, Rhs>)
    {
        return lhs == rhs;
    }
    else
    {
        return false;
    }
}

} //namespace

#endif
