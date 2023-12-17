//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the version of the Maki library
*/

#ifndef MAKI_DETAIL_CONSTANT_HPP
#define MAKI_DETAIL_CONSTANT_HPP

namespace maki::detail
{

template<const auto& Value>
struct constant
{
    static constexpr const auto& value = Value;
};

template<const auto& Value>
constexpr auto constant_c = constant<Value>{};

} //namespace

#endif
