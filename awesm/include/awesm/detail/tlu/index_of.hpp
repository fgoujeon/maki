//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_TLU_INDEX_OF_HPP
#define AWESM_DETAIL_TLU_INDEX_OF_HPP

namespace awesm::detail::tlu
{

namespace index_of_detail
{
    //T != U
    template<class U, class T, class... Ts>
    struct index_of_in_type_pack
    {
        static constexpr int value = index_of_in_type_pack<U, Ts...>::value + 1;
    };

    //T == U
    template<class T, class... Ts>
    struct index_of_in_type_pack<T, T, Ts...>
    {
        static constexpr int value = 0;
    };
}

/*
index_of is the index of the given type in the given type list.

In this example, index == 2:
    using tuple = tuple<char, short, int, long>;
    constexpr auto index = index_of_v<tuple, int>;
*/
template<class U, class TList>
struct index_of;

template<class U, template<class...> class TList, class... Ts>
struct index_of<U, TList<Ts...>>
{
    static constexpr int value =
        index_of_detail::index_of_in_type_pack<U, Ts...>::value
    ;
};

template<class TList, class U>
constexpr int index_of_v = index_of<U, TList>::value;

} //namespace

#endif
