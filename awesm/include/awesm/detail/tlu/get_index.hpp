//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_TLU_GET_INDEX_HPP
#define AWESM_DETAIL_TLU_GET_INDEX_HPP

namespace awesm::detail::tlu
{

/*
get_index returns the index of the given type in the given type list

In this example, index == 2:
    using tuple = tuple<char, short, int, long>;
    constexpr auto index = get_index<tuple, int>;
*/
namespace get_index_detail
{
    //T != U
    template<class U, class T, class... Ts>
    struct get_index_in_type_pack
    {
        static constexpr int value = get_index_in_type_pack<U, Ts...>::value + 1;
    };

    //T == U
    template<class T, class... Ts>
    struct get_index_in_type_pack<T, T, Ts...>
    {
        static constexpr int value = 0;
    };

    template<class U, class TList>
    struct get_index_in_type_list;

    template<class U, template<class...> class TList, class... Ts>
    struct get_index_in_type_list<U, TList<Ts...>>
    {
        static constexpr int value = get_index_in_type_pack<U, Ts...>::value;
    };
}

template<class TList, class U>
constexpr int get_index =
    get_index_detail::get_index_in_type_list<U, TList>::value
;

} //namespace

#endif
