//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_ID_TRAITS_HPP
#define MAKI_DETAIL_STATE_ID_TRAITS_HPP

#include <type_traits>

namespace maki::detail::state_id_traits
{

template<const auto& StateMold>
struct context
{
    using type = typename std::decay_t<decltype(impl_of(StateMold))>::context_type;
};

template<const auto& StateMold>
using context_t = typename context<StateMold>::type;

template<const auto& StateMold>
constexpr auto has_context_v = !std::is_void_v<context_t<StateMold>>;

} //namespace

#endif
