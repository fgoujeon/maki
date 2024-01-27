//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TU_APPLY_HPP
#define MAKI_DETAIL_TU_APPLY_HPP

#include "get.hpp"
#include <utility>

namespace maki::detail::tu
{

template<class IndexSequence>
struct apply_impl;

template<int... Indexes>
struct apply_impl<std::integer_sequence<int, Indexes...>>
{
    template<class Tuple, class F>
    static constexpr auto call(Tuple& tpl, const F& fun)
    {
        return fun(get<Indexes>(tpl)...);
    }

    template<class Tuple, class F, class... ExtraArgs>
    static constexpr auto call(Tuple& tpl, const F& fun, ExtraArgs&&... extra_args)
    {
        return fun(std::forward<ExtraArgs>(extra_args)..., get<Indexes>(tpl)...);
    }
};

template<class Tuple, class F>
constexpr auto apply(Tuple& tpl, const F& fun)
{
    using impl_t = apply_impl<std::make_integer_sequence<int, Tuple::size>>;
    return impl_t::call(tpl, fun);
}

template<class Tuple, class F, class... ExtraArgs>
constexpr auto apply(Tuple& tpl, const F& fun, ExtraArgs&&... extra_args)
{
    using impl_t = apply_impl<std::make_integer_sequence<int, Tuple::size>>;
    return impl_t::call(tpl, fun, std::forward<ExtraArgs>(extra_args)...);
}

} //namespace

#endif
