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

template<std::size_t Index, class T>
class tuple_2_element
{
public:
    T& get_element(const std::integral_constant<std::size_t, Index> /*tag*/)
    {
        return value;
    }

    const T& get_element(const std::integral_constant<std::size_t, Index> /*tag*/) const
    {
        return value;
    }

    T value;
};

template<class IndexSequence, class... Ts>
class tuple_2_base;

template<std::size_t... Indexes, class... Ts>
class tuple_2_base<std::index_sequence<Indexes...>, Ts...>:
    public tuple_2_element<Indexes, Ts>...
{
public:
    using tuple_2_element<Indexes, Ts>::get_element...;
};

template<class... Ts>
class tuple_2: private tuple_2_base<std::make_index_sequence<sizeof...(Ts)>, Ts...>
{
public:
    template<class... Args>
    constexpr tuple_2(Args&&... args):
        base_type{std::forward<Args>(args)...}
    {
    }

    template<std::size_t Index>
    constexpr auto& get()
    {
        return get_element(std::integral_constant<std::size_t, Index>{});
    }

    template<std::size_t Index>
    constexpr const auto& get() const
    {
        return get_element(std::integral_constant<std::size_t, Index>{});
    }

private:
    using base_type = tuple_2_base<std::make_index_sequence<sizeof...(Ts)>, Ts...>;

    using base_type::get_element;
};

template<>
class tuple_2<>
{
public:
};

template<class... Args>
tuple_2(Args&&... args) -> tuple_2<std::decay_t<Args>...>;

template<class... Ts, class U>
constexpr bool contains(const tuple_2<Ts...>& tpl, const U& elem)
{
    constexpr auto equals = [](const auto& a, const auto& b)
    {
        if constexpr(std::is_same_v<decltype(a), decltype(b)>)
        {
            return a == b;
        }
        else
        {
            return false;
        }
    };

    return (equals(get<Ts>(tpl), elem) || ...);
}

template<class IndexSequence>
struct for_each_element_helper;

template<std::size_t... Indexes>
struct for_each_element_helper<std::index_sequence<Indexes...>>
{
    template<class Tuple, class F>
    static void call(const Tuple& tpl, F&& fun)
    {
        (fun(tpl.template get<Indexes>()), ...);
    }
};

template<class... Ts, class F>
void for_each_element(const tuple_2<Ts...>& tpl, F&& fun)
{
    for_each_element_helper<std::make_index_sequence<sizeof...(Ts)>>::call(tpl, fun);
}

template<class... Ts, class U>
constexpr auto push_back(const tuple_2<Ts...>& tpl, const U& elem)
{
    return tuple_2{tpl, elem};
}

} //namespace

#endif
