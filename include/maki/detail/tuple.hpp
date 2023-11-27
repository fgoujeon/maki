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

template<int Index, class T>
struct tuple_element
{
    template<class... Args>
    explicit constexpr tuple_element(Args&... args):
        value{args...}
    {
    }

    T value;
};

template<class IndexSequence, class... Ts>
struct tuple_base;

template<int... Indexes, class... Ts>
struct tuple_base<std::integer_sequence<int, Indexes...>, Ts...>:
    tuple_element<Indexes, Ts>...
{
    constexpr tuple_base() = default;

    constexpr tuple_base(const tuple_base& other) = default;

    constexpr tuple_base(tuple_base&& other) = delete;

    template<class... Args>
    explicit constexpr tuple_base(distributed_construct_t /*tag*/, Args&... args):
        tuple_element<Indexes, Ts>{args}...
    {
    }

    template<class Arg>
    constexpr tuple_base(uniform_construct_t /*tag*/, Arg& arg):
        tuple_element<Indexes, Ts>{arg}...
    {
    }

    template<class Arg0, class Arg1>
    constexpr tuple_base(uniform_construct_t /*tag*/, Arg0& arg0, Arg1& arg1):
        tuple_element<Indexes, Ts>{arg0, arg1}...
    {
    }

    ~tuple_base() = default;

    tuple_base& operator=(const tuple_base& other) = delete;

    tuple_base& operator=(tuple_base&& other) = delete;
};

/*
A minimal std::tuple-like container.
Using this instead of std::tuple improves build time.
*/
template<class... Ts>
struct tuple: tuple_base<std::make_integer_sequence<int, sizeof...(Ts)>, Ts...>
{
    static constexpr auto size = sizeof...(Ts);

    using tuple_base<std::make_integer_sequence<int, sizeof...(Ts)>, Ts...>::tuple_base;
};

template<int Index, class Tuple>
constexpr auto& tuple_get(Tuple& tpl)
{
    using tuple_t = std::decay_t<Tuple>;
    using type_t = tlu::get_t<tuple_t, Index>;
    using element_type_t = tuple_element<Index, type_t>;
    return tpl.element_type_t::value;
}

template<class T, class Tuple>
constexpr auto& tuple_get(Tuple& tpl)
{
    using tuple_t = std::decay_t<Tuple>;
    constexpr auto element_index = tlu::index_of_v<tuple_t, T>;
    using element_type_t = tuple_element<element_index, T>;
    return tpl.element_type_t::value;
}

template<class IndexSequence>
struct tuple_append_impl;

template<int... Indexes>
struct tuple_append_impl<std::integer_sequence<int, Indexes...>>
{
    template<class... Ts, class U>
    static constexpr tuple<Ts..., U> call(const tuple<Ts...>& tpl, const U& elem)
    {
        return tuple<Ts..., U>{distributed_construct, tuple_get<Indexes>(tpl)..., elem};
    }
};

template<class... Ts, class U>
constexpr tuple<Ts..., U> tuple_append(const tuple<Ts...>& tpl, const U& elem)
{
    using impl_t = tuple_append_impl<std::make_integer_sequence<int, sizeof...(Ts)>>;
    return impl_t::call(tpl, elem);
}

template<class IndexSequence>
struct tuple_apply_impl;

template<int... Indexes>
struct tuple_apply_impl<std::integer_sequence<int, Indexes...>>
{
    template<class Tuple, class F, class... ExtraArgs>
    static auto call(Tuple& tpl, const F& fun, ExtraArgs&&... extra_args)
    {
        return fun(std::forward<ExtraArgs>(extra_args)..., tuple_get<Indexes>(tpl)...);
    }
};

template<class Tuple, class F, class... ExtraArgs>
auto tuple_apply(Tuple& tpl, const F& fun, ExtraArgs&&... extra_args)
{
    using impl_t = tuple_apply_impl<std::make_integer_sequence<int, Tuple::size>>;
    return impl_t::call(tpl, fun, std::forward<ExtraArgs>(extra_args)...);
}

} //namespace

#endif
