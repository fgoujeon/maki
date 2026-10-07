// Copyright Florian Goujeon 2021 - 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
// Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TUPLE_HPP
#define MAKI_DETAIL_TUPLE_HPP

#include "constant.hpp"
#include "tlu.hpp"
#include "iseq.hpp"
#include <utility>

namespace maki::detail
{

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

    constexpr T& value()
    {
        return value_;
    }

    constexpr const T& value() const
    {
        return value_;
    }

private:
    T value_;
};

template<class... Ts>
class tuple;

template<class IndexSequence, class... Ts>
class tuple_base;

template<int... Indexes, class... Ts>
class tuple_base<iseq<Indexes...>, Ts...>
    : private tuple_element<Indexes, Ts>...
{
public:
    constexpr tuple_base() = default;

    constexpr tuple_base(const tuple_base& other) = default;

    constexpr tuple_base(tuple_base&& other) = delete;

    template<class... Args>
    explicit constexpr tuple_base(Args&&... args):
        tuple_element<Indexes, Ts>{std::forward<Args>(args)}...
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

    template<class U>
    constexpr auto append(const U& elem) const
    {
        return tuple<Ts..., U>{tuple_element<Indexes, Ts>::value()..., elem};
    }
};

/*
A minimal std::tuple-like container.
Using this instead of std::tuple improves build time.
*/
template<class... Ts>
class tuple
    : public tuple_base<linear_iseq_t<sizeof...(Ts)>, Ts...>
{
public:
    using base_t =
        tuple_base<linear_iseq_t<sizeof...(Ts)>, Ts...>;

    using base_t::base_t;
    using base_t::get;

    static constexpr auto size = sizeof...(Ts);
};

template<class... Args>
constexpr auto make_tuple(const Args&... args)
{
    return tuple<Args...>{args...};
}


/*
tuple_get
*/

template<int Index, class... Ts>
constexpr auto& tuple_get(tuple<Ts...>& tpl)
{
    return tpl.get(constant<Index>);
}

template<int Index, class... Ts>
constexpr const auto& tuple_get(const tuple<Ts...>& tpl)
{
    return tpl.get(constant<Index>);
}


/*
tuple_apply
*/

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
    static constexpr auto
    call(Tuple& tpl, const F& fun, ExtraArgs&&... extra_args)
    {
        return fun(
            std::forward<ExtraArgs>(extra_args)...,
            tuple_get<Indexes>(tpl)...);
    }
};

template<class Tuple, class F>
constexpr auto tuple_apply(Tuple& tpl, const F& fun)
{
    using impl_t =
        tuple_apply_impl<std::make_integer_sequence<int, Tuple::size>>;
    return impl_t::call(tpl, fun);
}

template<class Tuple, class F, class... ExtraArgs>
constexpr auto tuple_apply(Tuple& tpl, const F& fun, ExtraArgs&&... extra_args)
{
    using impl_t =
        tuple_apply_impl<std::make_integer_sequence<int, Tuple::size>>;
    return impl_t::call(tpl, fun, std::forward<ExtraArgs>(extra_args)...);
}

} // namespace maki::detail

#endif
