//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TUPLE_HPP
#define MAKI_DETAIL_TUPLE_HPP

#include "tlu.hpp"

namespace maki::detail
{

/*
A minimal std::tuple-like container.
Using this instead of std::tuple improves build time.
*/

template<class... Ts>
struct tuple: Ts...
{
    template<class Arg>
    explicit tuple(Arg& arg):
        Ts(arg)...
    {
    }

    template<class Arg0, class Arg1>
    explicit tuple(Arg0& arg0, Arg1& arg1):
        Ts(arg0, arg1)...
    {
    }
};

template<class T, class... Ts>
T& get(tuple<Ts...>& tpl)
{
    return static_cast<T&>(tpl);
}

template<class T, class... Ts>
const T& get(const tuple<Ts...>& tpl)
{
    return static_cast<const T&>(tpl);
}

template<int Index, class... Ts>
auto& get(tuple<Ts...>& tpl)
{
    using type_t = tlu::get_t<tuple<Ts...>, Index>;
    return get<type_t>(tpl);
}

template<int Index, class... Ts>
const auto& get(const tuple<Ts...>& tpl)
{
    using type_t = tlu::get_t<tuple<Ts...>, Index>;
    return get<type_t>(tpl);
}

} //namespace

#endif
