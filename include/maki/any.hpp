//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_ANY_HPP
#define MAKI_ANY_HPP

#include <type_traits>

namespace maki
{

struct any_t{};

inline constexpr auto any = any_t{};

namespace detail
{
    template<class T>
    constexpr bool is_any_v = std::is_same_v<T, any_t>;
}

} //namespace

#endif
