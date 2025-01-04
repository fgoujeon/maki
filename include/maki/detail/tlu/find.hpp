//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_FIND_HPP
#define MAKI_DETAIL_TLU_FIND_HPP

namespace maki::detail::tlu
{

namespace find_detail
{
    //T != U
    template<class U, class T, class... Ts>
    struct find_in_type_pack
    {
        static constexpr int value = find_in_type_pack<U, Ts...>::value + 1;
    };

    //T == U
    template<class T, class... Ts>
    struct find_in_type_pack<T, T, Ts...>
    {
        static constexpr int value = 0;
    };
}

/*
find is the index of the given type in the given type list.

In this example, index == 2:
    using tuple = tuple<char, short, int, long>;
    constexpr auto index = find_v<tuple, int>;
*/
template<class U, class TList>
struct find;

template<class U, template<class...> class TList, class... Ts>
struct find<U, TList<Ts...>>
{
    static constexpr int value =
        find_detail::find_in_type_pack<U, Ts...>::value
    ;
};

template<class TList, class U>
constexpr int find_v = find<U, TList>::value;

} //namespace

#endif
