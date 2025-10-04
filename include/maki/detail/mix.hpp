//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_MIX_HPP
#define MAKI_DETAIL_MIX_HPP

#include "constant.hpp"
#include "type_list.hpp"
#include <type_traits>

namespace maki::detail
{

struct mix_uniform_construct_t{};
constexpr mix_uniform_construct_t mix_uniform_construct;

struct mix_distributed_construct_t{};
constexpr mix_distributed_construct_t mix_distributed_construct;

/*
An ultra-minimalist tuple that can only be used when all types are different.
*/
template<class... Ts>
struct mix: Ts...
{
    constexpr mix() = default;

    constexpr mix(mix_distributed_construct_t /*tag*/, const Ts&... elems):
        Ts{elems}...
    {
    }

    template<class Arg0>
    constexpr mix(mix_uniform_construct_t /*tag*/, Arg0& arg0):
        Ts{arg0}...
    {
    }

    template<class Arg0, class Arg1>
    constexpr mix(mix_uniform_construct_t /*tag*/, Arg0& arg0, Arg1& arg1):
        Ts{arg0, arg1}...
    {
    }

    static constexpr auto size = sizeof...(Ts);
};

template<class... Ts, class U>
constexpr auto append(const mix<Ts...>& imix, const U& elem)
{
    return mix<Ts..., U>
    {
        mix_distributed_construct,
        static_cast<const Ts&>(imix)...,
        elem
    };
}

template<class T, class... Ts>
constexpr T& get(mix<Ts...>& imix)
{
    return static_cast<T&>(imix);
}

template<class T, class... Ts>
constexpr const T& get(const mix<Ts...>& imix)
{
    return static_cast<const T&>(imix);
}


/*
mix_elem_c
*/

template<const auto& Mix, class T>
constexpr auto mix_elem_c = static_cast<const T&>(Mix);


/*
mix_constant_list_t
*/

template<class Mix>
struct mix_constant_list_helper;

template<class... Ts>
struct mix_constant_list_helper<mix<Ts...>>
{
    template<const mix<Ts...>& Mix>
    using type = type_list_t<constant_t<&mix_elem_c<Mix, Ts>>...>;
};

template<const auto& Mix>
using mix_constant_list_t =
    typename mix_constant_list_helper
    <
        std::decay_t<decltype(Mix)>
    >::template type<Mix>
;

} //namespace

#endif
