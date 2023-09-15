//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TUPLE_2_HPP
#define MAKI_DETAIL_TUPLE_2_HPP

#include "tlu.hpp"

namespace maki::detail
{

/*
A minimal std::tuple-like container.
Using this instead of std::tuple improves build time.
*/

template<class T>
struct tuple_2_element
{
    T value;
};

template<class... Ts>
struct tuple_2: tuple_2_element<Ts>...
{
    template<class... Args>
    tuple_2(Args&&... args):
        tuple_2_element<Ts>{args}...
    {
    }

    template<class U>
    constexpr auto add(const U& elem) const
    {
        return make_tuple_2(tuple_2_element<Ts>::value..., elem);
    }
};

template<class... Args>
constexpr auto make_tuple_2(Args&&... args)
{
    return tuple_2<std::decay_t<Args>...>{std::forward<Args>(args)...};
}

template<class T, class... Ts>
const T& get(const tuple_2<Ts...>& tpl)
{
    return static_cast<const tuple_2_element<T>&>(tpl).value;
}

template<class... Ts, class F>
void for_each_element(const tuple_2<Ts...>& tpl, F&& fun)
{
    (fun(get<Ts>(tpl)), ...);
}

} //namespace

#endif
