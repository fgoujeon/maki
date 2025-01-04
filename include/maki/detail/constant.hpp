//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_CONSTANT_HPP
#define MAKI_DETAIL_CONSTANT_HPP

namespace maki::detail
{

template<auto Value>
struct constant_t
{
    static constexpr auto value = Value;
};

template<auto Value>
constexpr auto constant = constant_t<Value>{};

} //namespace

#endif
