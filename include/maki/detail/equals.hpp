//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_EQUALS_HPP
#define MAKI_DETAIL_EQUALS_HPP

namespace maki::detail
{

template<class Lhs, class Rhs>
constexpr bool equals(const Lhs& /*lhs*/, const Rhs& /*rhs*/)
{
    return false;
}

template<class T>
constexpr bool equals(const T& lhs, const T& rhs)
{
    return lhs == rhs;
}

template<class Lhs, class Rhs>
constexpr bool ptr_equals(const Lhs* plhs, const Rhs* prhs)
{
    return static_cast<const void*>(plhs) == prhs;
}

} //namespace

#endif
