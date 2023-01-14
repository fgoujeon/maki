//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_TLU_CONTAINS_HPP
#define AWESM_DETAIL_TLU_CONTAINS_HPP

namespace awesm::detail::tlu
{

/*
contains is a boolean indicating whether the given typelist contains the given
type.

In this example...:
    using type_list = std::tuple<char, short, int, long>;
    constexpr auto contains_int = contains<type_list, int>;

... contains_int == true.
*/

namespace contains_detail
{
    template<class U, class... Ts>
    struct contains_in_type_pack;

    //terminal case
    template<class U>
    struct contains_in_type_pack<U>
    {
        static constexpr bool value = false;
    };

    //U == T
    template<class U, class... Ts>
    struct contains_in_type_pack<U, U, Ts...>
    {
        static constexpr bool value = true;
    };

    //U != T
    template<class U, class T, class... Ts>
    struct contains_in_type_pack<U, T, Ts...>
    {
        static constexpr bool value = contains_in_type_pack<U, Ts...>::value;
    };

    template<class U, class TList>
    struct contains_in_type_list;

    template<class U, template<class...> class TList, class... Ts>
    struct contains_in_type_list<U, TList<Ts...>>
    {
        static constexpr bool value = contains_in_type_pack<U, Ts...>::value;
    };
}

template<class TList, class U>
constexpr bool contains_v =
    contains_detail::contains_in_type_list<U, TList>::value
;

} //namespace

#endif
