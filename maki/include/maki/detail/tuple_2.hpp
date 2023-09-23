//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TUPLE_2_HPP
#define MAKI_DETAIL_TUPLE_2_HPP

#include <functional>
#include <utility>
#include <type_traits>

namespace maki::detail
{

/*
A minimal std::tuple-like container.
Using this instead of std::tuple improves build time.
*/

template<std::size_t Index, class T>
class tuple_2_element
{
public:
    constexpr T& get_element(const std::integral_constant<std::size_t, Index> /*tag*/)
    {
        return value;
    }

    constexpr const T& get_element(const std::integral_constant<std::size_t, Index> /*tag*/) const
    {
        return value;
    }

    T value;
};

template<class IndexSequence, class... Ts>
class tuple_2_impl;

template<std::size_t... Indexes, class... Ts>
class tuple_2_impl<std::index_sequence<Indexes...>, Ts...>:
    public tuple_2_element<Indexes, Ts>...
{
public:
    using tuple_2_element<Indexes, Ts>::get_element...;
};

template<class... Ts>
class tuple_2
{
public:
    explicit constexpr tuple_2(const Ts&... args):
        impl_{args...}
    {
    }

    template<std::size_t Index>
    constexpr auto& get()
    {
        return impl_.get_element(std::integral_constant<std::size_t, Index>{});
    }

    template<std::size_t Index>
    constexpr const auto& get() const
    {
        return impl_.get_element(std::integral_constant<std::size_t, Index>{});
    }

private:
    tuple_2_impl<std::make_index_sequence<sizeof...(Ts)>, Ts...> impl_;
};

template<class... Ts>
constexpr tuple_2<Ts...> make_tuple_2(const Ts&... elems)
{
    return tuple_2<Ts...>{elems...};
}


/*
get
*/

template<std::size_t Index, class... Ts>
constexpr const auto& get(const tuple_2<Ts...>& tpl)
{
    return tpl.template get<Index>();
}


/*
apply
*/

template<class IndexSequence>
struct apply_helper;

template<std::size_t... Indexes>
struct apply_helper<std::index_sequence<Indexes...>>
{
    template<class F, class... Ts, class... Args>
    static constexpr auto call(F&& fun, const tuple_2<Ts...>& tpl, Args&&... args)
    {
        return std::forward<F>(fun)
        (
            get<Indexes>(tpl)...,
            std::forward<Args>(args)...
        );
    }
};

template<class F, class... Ts, class... Args>
constexpr auto apply(F&& fun, const tuple_2<Ts...>& tpl, Args&&... args)
{
    using index_sequence_t = std::make_index_sequence<sizeof...(Ts)>;
    return apply_helper<index_sequence_t>::call
    (
        std::forward<F>(fun),
        tpl,
        std::forward<Args>(args)...
    );
}


/*
for_each_element
*/

template<class IndexSequence>
struct for_each_element_helper;

template<std::size_t... Indexes>
struct for_each_element_helper<std::index_sequence<Indexes...>>
{
    template<class F, class... Ts, class... Args>
    static void call(F&& fun, const tuple_2<Ts...>& tpl, Args&&... args)
    {
        (
            fun
            (
                get<Indexes>(tpl),
                std::forward<Args>(args)...
            ),
            ...
        );
    }
};

template<class F, class... Ts, class... Args>
void for_each_element(F&& fun, const tuple_2<Ts...>& tpl, Args&&... args)
{
    for_each_element_helper<std::make_index_sequence<sizeof...(Ts)>>::call
    (
        std::forward<F>(fun),
        tpl,
        std::forward<Args>(args)...
    );
}


/*
for_each_element_or
*/

template<class IndexSequence>
struct for_each_element_or_helper;

template<std::size_t... Indexes>
struct for_each_element_or_helper<std::index_sequence<Indexes...>>
{
    template<class F, class... Ts, class... Args>
    static bool call(F&& fun, const tuple_2<Ts...>& tpl, Args&&... args)
    {
        return (
            std::invoke
            (
                std::forward<F>(fun),
                tpl.template get<Indexes>(),
                std::forward<Args>(args)...
            ) ||
            ...
        );
    }
};

template<class F, class... Ts, class... Args>
bool for_each_element_or(F&& fun, const tuple_2<Ts...>& tpl, Args&&... args)
{
    return for_each_element_or_helper<std::make_index_sequence<sizeof...(Ts)>>::call
    (
        std::forward<F>(fun),
        tpl,
        std::forward<Args>(args)...
    );
}


/*
push_back
*/

template<class... Ts, class U>
constexpr auto push_back(const tuple_2<Ts...>& tpl, const U& elem)
{
    return apply
    (
        make_tuple_2<Ts..., U>,
        tpl,
        elem
    );
}

} //namespace

#endif
