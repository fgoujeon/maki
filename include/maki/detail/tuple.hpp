//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TUPLE_HPP
#define MAKI_DETAIL_TUPLE_HPP

#include "tlu.hpp"
#include "constant.hpp"
#include "../type_list.hpp"
#include "type.hpp"
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
        value_{std::forward<Args>(args)...}
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
        tuple_element<Indexes, Ts>{std::forward<Args>(args)}...
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

    /*
    We need this dummy function in case Ts is empty. `tuple` does a using
    base_t::get, so there must be at least one get() function defined here.
    */
    void get()
    {
    }

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

template<const auto& Tuple, class IndexSequence>
struct tuple_to_element_ptr_constant_list_impl;

template<const auto& Tuple, int... Indexes>
struct tuple_to_element_ptr_constant_list_impl<Tuple, std::integer_sequence<int, Indexes...>>
{
    using type = type_list_t<constant_t<&tuple_static_get_copy_c<Tuple, Indexes>>...>;
};

template<const auto& Tuple>
struct tuple_to_element_ptr_constant_list
{
    using type = typename tuple_to_element_ptr_constant_list_impl<Tuple, std::make_integer_sequence<int, Tuple.size>>::type;
};

template<const auto& Tuple>
using tuple_to_element_ptr_constant_list_t = typename tuple_to_element_ptr_constant_list<Tuple>::type;

} //namespace

#endif
