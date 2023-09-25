//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_CONSTANT_HPP
#define MAKI_DETAIL_CONSTANT_HPP

namespace maki::detail
{

template<auto Value>
struct constant
{
    static constexpr auto value = Value;
};

template<class T, T Value>
struct constant_2
{
    static constexpr T value = Value;
};

template<auto Value>
inline constexpr auto constant_c = std::integral_constant<decltype(Value), Value>{};

template<const auto& Value>
inline constexpr auto constant_reference_c = std::integral_constant<decltype(Value), Value>{};

} //namespace

#endif
