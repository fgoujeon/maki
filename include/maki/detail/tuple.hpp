//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TUPLE_HPP
#define MAKI_DETAIL_TUPLE_HPP

#include "tlu.hpp"
#include <utility>

namespace maki::detail
{

struct uniform_construct_t{};
inline constexpr auto uniform_construct = uniform_construct_t{};

struct distributed_construct_t{};
inline constexpr auto distributed_construct = distributed_construct_t{};

template<class... Ts>
struct tuple;

template<class T, class... Ts>
T& get(tuple<Ts...>& tpl)
{
    return static_cast<T&>(tpl);
}

template<class T, class... Ts>
constexpr const T& get(const tuple<Ts...>& tpl)
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

template<class... Ts, class U>
constexpr tuple<Ts..., U> append(const tuple<Ts...>& tpl, const U& elem)
{
    return tuple<Ts..., U>{distributed_construct, get<Ts>(tpl)..., elem};
}

template<class... Ts, class F, class... ExtraArgs>
auto apply(const tuple<Ts...>& tpl, const F& fun, ExtraArgs&&... extra_args)
{
    return fun(std::forward<ExtraArgs>(extra_args)..., get<Ts>(tpl)...);
}

/*
A minimal std::tuple-like container.
Using this instead of std::tuple improves build time.
*/
template<class... Ts>
struct tuple: Ts...
{
    constexpr tuple() = default;

    template<class... Args>
    explicit constexpr tuple(distributed_construct_t /*tag*/, const Args&... args):
        Ts(args)...
    {
    }

    template<class Arg>
    constexpr tuple(uniform_construct_t /*tag*/, Arg& arg):
        Ts(arg)...
    {
    }

    template<class Arg0, class Arg1>
    constexpr tuple(uniform_construct_t /*tag*/, Arg0& arg0, Arg1& arg1):
        Ts(arg0, arg1)...
    {
    }
};

} //namespace

#endif
