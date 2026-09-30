// Copyright Florian Goujeon 2021 - 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
// Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TYPE_BIMAP_HPP
#define MAKI_DETAIL_TYPE_BIMAP_HPP

#include "type.hpp"
#include "type_list.hpp"

namespace maki::detail
{

/*
type_bimap
*/

template<class L, class R>
struct type_bimap_element
{
    static constexpr type_t<R> get_r(type_t<L> /*tag*/)
    {
        return {};
    }

    static constexpr type_t<L> get_l(type_t<R> /*tag*/)
    {
        return {};
    }
};

template<class LList = type_list_t<>, class RList = type_list_t<>>
struct type_bimap;

template<
    template<class...> class LList,
    class... Ls,
    template<class...> class RList,
    class... Rs>
struct type_bimap<LList<Ls...>, RList<Rs...>>: type_bimap_element<Ls, Rs>...
{
    using type_bimap_element<Ls, Rs>::get_r...;
    using type_bimap_element<Ls, Rs>::get_l...;

    using left_type_list = LList<Ls...>;
    using right_type_list = RList<Rs...>;

    template<class L, class R>
    using insert = type_bimap<LList<Ls..., L>, RList<Rs..., R>>;
};


/*
type_bimap_get
*/

template<class Bimap, class L>
using type_bimap_get_r_t = typename decltype(Bimap::get_r(type<L>))::type;

template<class Bimap, class R>
using type_bimap_get_l_t = typename decltype(Bimap::get_l(type<R>))::type;


/*
type_bimap_insert_if
*/

template<class Bimap, class L, class R, bool Condition>
struct type_bimap_insert_if
{
    using type = Bimap;
};

template<class Bimap, class L, class R>
struct type_bimap_insert_if<Bimap, L, R, true>
{
    using type = typename Bimap::template insert<L, R>;
};

template<class Bimap, class L, class R, bool Condition>
using type_bimap_insert_if_t =
    typename type_bimap_insert_if<Bimap, L, R, Condition>::type;

} // namespace maki::detail

#endif
