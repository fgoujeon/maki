//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TUPLE_HPP
#define MAKI_DETAIL_TUPLE_HPP

#include "tlu.hpp"
#include "type_list.hpp"
#include "constant.hpp"
#include "../cref_constant.hpp"
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

    using base_t::base_t;
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

template<int Index, class Tuple>
constexpr auto& tuple_get(Tuple& tpl)
{
    return tpl.get(constant<Index>);
}

template<class T, class Tuple>
constexpr auto& tuple_get(Tuple& tpl)
{
    return tpl.get(type<T>);
}

template<const auto& Tuple, int Index>
constexpr auto tuple_static_get_copy_c = tuple_get<Index>(Tuple);

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
    template<class Tuple, class F>
    static constexpr auto call(Tuple& tpl, const F& fun)
    {
        return fun(tuple_get<Indexes>(tpl)...);
    }

    template<class Tuple, class F, class... ExtraArgs>
    static constexpr auto call(Tuple& tpl, const F& fun, ExtraArgs&&... extra_args)
    {
        return fun(std::forward<ExtraArgs>(extra_args)..., tuple_get<Indexes>(tpl)...);
    }
};

template<class Tuple, class F>
constexpr auto tuple_apply(Tuple& tpl, const F& fun)
{
    using impl_t = tuple_apply_impl<std::make_integer_sequence<int, Tuple::size>>;
    return impl_t::call(tpl, fun);
}

template<class Tuple, class F, class... ExtraArgs>
constexpr auto tuple_apply(Tuple& tpl, const F& fun, ExtraArgs&&... extra_args)
{
    using impl_t = tuple_apply_impl<std::make_integer_sequence<int, Tuple::size>>;
    return impl_t::call(tpl, fun, std::forward<ExtraArgs>(extra_args)...);
}

inline constexpr auto tuple_contains_if_impl = [](const auto& pred, const auto&... elems)
{
    return (pred(elems) || ...);
};

template<class Tuple, class Predicate>
constexpr bool tuple_contains_if(Tuple& tpl, const Predicate& pred)
{
    return tuple_apply(tpl, tuple_contains_if_impl, pred);
}

template<const auto& Tuple, class IndexSequence>
struct tuple_to_constant_list_impl;

template<const auto& Tuple, int... Indexes>
struct tuple_to_constant_list_impl<Tuple, std::integer_sequence<int, Indexes...>>
{
    using type = type_list<cref_constant_t<tuple_static_get_copy_c<Tuple, Indexes>>...>;
};

template<const auto& Tuple>
struct tuple_to_constant_list
{
    using type = typename tuple_to_constant_list_impl<Tuple, std::make_integer_sequence<int, Tuple.size>>::type;
};

template<const auto& Tuple>
using tuple_to_constant_list_t = typename tuple_to_constant_list<Tuple>::type;

} //namespace

#endif
