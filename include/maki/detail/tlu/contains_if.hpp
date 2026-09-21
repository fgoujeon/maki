// Copyright Florian Goujeon 2021 - 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
// Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_CONTAINS_IF_HPP
#define MAKI_DETAIL_TLU_CONTAINS_IF_HPP

namespace maki::detail::tlu
{

/*
contains_if is a boolean indicating whether the given typelist contains the
given type.

In this example...:
    using type_list_t = std::tuple<char, short, int, long>;
    constexpr auto contains_if_int = contains_if<type_list_t, int>;

... contains_if_int == true.
*/

template<class TList, template<class> class Predicate>
struct contains_if;

template<
    template<class...> class TList,
    class... Ts,
    template<class> class Predicate>
struct contains_if<TList<Ts...>, Predicate>
{
    static constexpr bool value = (Predicate<Ts>::value || ...);
};

template<class TList, template<class> class Predicate>
constexpr bool contains_if_v = contains_if<TList, Predicate>::value;

} // namespace maki::detail::tlu

#endif
