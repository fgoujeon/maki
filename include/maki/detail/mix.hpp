//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_MIX_HPP
#define MAKI_DETAIL_MIX_HPP

#include "constant.hpp"
#include "type_list.hpp"

namespace maki::detail
{

/*
A kind of simple tuple that can only be used when all types are different.
*/
template<class... Ts>
struct mix: Ts...
{
    static constexpr auto size = sizeof...(Ts);
};

template<class... Ts, class U>
constexpr auto append(const mix<Ts...>& tpl, const U& elem)
{
    return mix<Ts..., U>
    {
        static_cast<const Ts&>(tpl)...,
        elem
    };
}


/*
mix_elem_c
*/

template<const auto& Mix, class T>
constexpr auto mix_elem_c = static_cast<const T&>(Mix);


/*
mix_constant_list_t
*/

template<class Mix, const Mix& Mx>
struct mix_constant_list;

template<class... Ts, const mix<Ts...>& Mx>
struct mix_constant_list<mix<Ts...>, Mx>
{
    using type = type_list_t<constant_t<&mix_elem_c<Mx, Ts>>...>;
};

template<const auto& Mix>
using mix_constant_list_t =
    typename mix_constant_list<std::decay_t<decltype(Mix)>, Mix>::type;

} //namespace

#endif
