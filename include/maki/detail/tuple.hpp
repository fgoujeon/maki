//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TUPLE_HPP
#define MAKI_DETAIL_TUPLE_HPP

#include "constant.hpp"
#include "../type.hpp"
#include <utility>

namespace maki::detail
{

struct uniform_construct_t{};
inline constexpr auto uniform_construct = uniform_construct_t{};

struct distributed_construct_t{};
inline constexpr auto distributed_construct = distributed_construct_t{};

template<int Index, class T>
class tuple_element
{
public:
    template<class... Args>
    explicit constexpr tuple_element(Args&&... args):
        value_{args...}
    {
    }

    constexpr T& get(constant_t<Index> /*tag*/)
    {
        return value_;
    }

    constexpr const T& get(constant_t<Index> /*tag*/) const
    {
        return value_;
    }

    constexpr T& get(type_t<T> /*tag*/)
    {
        return value_;
    }

    constexpr const T& get(type_t<T> /*tag*/) const
    {
        return value_;
    }

private:
    T value_;
};

template<class IndexSequence, class... Ts>
class tuple_base;

template<int... Indexes, class... Ts>
class tuple_base<std::integer_sequence<int, Indexes...>, Ts...>:
    private tuple_element<Indexes, Ts>...
{
public:
    constexpr tuple_base() = default;

    constexpr tuple_base(const tuple_base& other) = default;

    constexpr tuple_base(tuple_base&& other) = delete;

    template<class... Args>
    explicit constexpr tuple_base(distributed_construct_t /*tag*/, Args&&... args):
        tuple_element<Indexes, Ts>{args}...
    {
    }

    template<class Arg>
    constexpr tuple_base(uniform_construct_t /*tag*/, Arg&& arg):
        tuple_element<Indexes, Ts>{arg}...
    {
    }

    template<class Arg0, class Arg1>
    constexpr tuple_base(uniform_construct_t /*tag*/, Arg0&& arg0, Arg1&& arg1):
        tuple_element<Indexes, Ts>{arg0, arg1}...
    {
    }

    template<class Arg0, class Arg1, class Arg2>
    constexpr tuple_base(uniform_construct_t /*tag*/, Arg0&& arg0, Arg1&& arg1, Arg2&& arg2):
        tuple_element<Indexes, Ts>{arg0, arg1, arg2}...
    {
    }

    ~tuple_base() = default;

    tuple_base& operator=(const tuple_base& other) = delete;

    tuple_base& operator=(tuple_base&& other) = delete;

    using tuple_element<Indexes, Ts>::get...;
};

/*
A minimal std::tuple-like container.
Using this instead of std::tuple improves build time.
*/
template<class... Ts>
class tuple: private tuple_base<std::make_integer_sequence<int, sizeof...(Ts)>, Ts...>
{
public:
    using base_t = tuple_base<std::make_integer_sequence<int, sizeof...(Ts)>, Ts...>;

    using base_t::tuple_base;
    using base_t::get;

    static constexpr auto size = sizeof...(Ts);
};

template<>
class tuple<>
{
public:
    constexpr tuple() = default;

    constexpr tuple(const tuple& other) = default;

    constexpr tuple(tuple&& other) = delete;

    template<class... Args>
    explicit constexpr tuple(distributed_construct_t /*tag*/, Args&&... /*args*/)
    {
    }

    template<class... Args>
    explicit constexpr tuple(uniform_construct_t /*tag*/, Args&&... /*args*/)
    {
    }

    ~tuple() = default;

    tuple& operator=(const tuple& other) = delete;

    tuple& operator=(tuple&& other) = delete;

    static constexpr auto size = 0;
};

template<class... Args>
constexpr auto make_tuple(distributed_construct_t /*tag*/, const Args&... args)
{
    return tuple<Args...>{distributed_construct, args...};
}

inline constexpr auto empty_tuple = make_tuple(distributed_construct);

template<class IndexSequence>
struct tuple_equality_impl;

template<int Index, int... Indexes>
struct tuple_equality_impl<std::integer_sequence<int, Index, Indexes...>>
{
    template<class... Args>
    static constexpr bool call(const tuple<Args...>& lhs, const tuple<Args...>& rhs)
    {
        return
            lhs.get(constant<Index>) == rhs.get(constant<Index>) &&
            tuple_equality_impl<std::integer_sequence<int, Indexes...>>::call(lhs, rhs);
        ;
    }
};

template<>
struct tuple_equality_impl<std::integer_sequence<int>>
{
    template<class... Args>
    static constexpr bool call(const tuple<Args...>& /*lhs*/, const tuple<Args...>& /*rhs*/)
    {
        return true;
    }
};

template<class... Args>
constexpr bool operator==(const tuple<Args...>& lhs, const tuple<Args...>& rhs)
{
    using impl_t = tuple_equality_impl<std::make_integer_sequence<int, static_cast<int>(sizeof...(Args))>>;
    return impl_t::call(lhs, rhs);
}

template<class... LhsArgs, class... RhsArgs>
constexpr bool operator==(const tuple<LhsArgs...>& /*lhs*/, const tuple<RhsArgs...>& /*rhs*/)
{
    return false;
}

} //namespace

#endif
