//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_NONE_HPP
#define MAKI_NONE_HPP

#include <type_traits>

namespace maki
{

struct none_t{};

inline constexpr auto none = none_t{};

namespace detail
{
    template<class T>
    constexpr bool is_none_v = std::is_same_v<T, none_t>;
}

} //namespace

#endif
