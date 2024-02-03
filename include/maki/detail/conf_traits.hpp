//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_CONF_TRAITS_HPP
#define MAKI_DETAIL_CONF_TRAITS_HPP

#include "../null.hpp"
#include <type_traits>

namespace maki::detail::conf_traits
{

template<const auto& Conf>
struct context
{
    using type = typename std::decay_t<decltype(opts(Conf))>::context_type;
};

template<const auto& Conf>
using context_t = typename context<Conf>::type;

template<const auto& Conf>
constexpr auto has_context_v = !std::is_void_v<context_t<Conf>>;

} //namespace

#endif
