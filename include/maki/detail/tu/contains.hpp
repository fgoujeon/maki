//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TU_CONTAINS_HPP
#define MAKI_DETAIL_TU_CONTAINS_HPP

#include "apply.hpp"

namespace maki::detail::tu
{

template<class Tuple, class T>
constexpr bool contains(const Tuple& tpl, const T& value)
{
    return apply
    (
        tpl,
        [](const T& value, const auto&... elems)
        {
            auto weak_equals = [](const auto& lhs, const auto& rhs)
            {
                if constexpr(std::is_same_v<decltype(lhs), decltype(rhs)>)
                {
                    return lhs == rhs;
                }
                else
                {
                    return false;
                }
            };

            return (weak_equals(elems, value) || ...);
        },
        value
    );
}

} //namespace

#endif
