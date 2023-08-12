//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_FILTER_HPP
#define MAKI_DETAIL_TLU_FILTER_HPP

namespace maki::detail::tlu
{

namespace filter_detail
{
    template
    <
        class TList,
        class UList
    >
    struct concat;

    template
    <
        template<class...> class TList,
        class... Ts,
        class... Us
    >
    struct concat<TList<Ts...>, TList<Us...>>
    {
        using type = TList<Ts..., Us...>;
    };

    template
    <
        template<class...> class TList,
        class T,
        bool B
    >
    struct make_type_list_from_type_if
    {
        using type = TList<T>;
    };

    template
    <
        template<class...> class TList,
        class T
    >
    struct make_type_list_from_type_if<TList, T, false>
    {
        using type = TList<>;
    };
}

template
<
    class TList,
    template<class> class Predicate
>
struct filter;

template
<
    template<class...> class TList,
    class T,
    class... Ts,
    template<class> class Predicate
>
struct filter<TList<T, Ts...>, Predicate>
{
    using type = typename filter_detail::concat
    <
        typename filter_detail::make_type_list_from_type_if<TList, T, Predicate<T>::value>::type,
        typename filter<TList<Ts...>, Predicate>::type
    >::type;
};

template
<
    template<class...> class TList,
    template<class> class Predicate
>
struct filter<TList<>, Predicate>
{
    using type = TList<>;
};

/*
filter takes a type list and a predicate, and returns a new type list whose
each type T verifies Predicate<T>::value == true.
*/
template<class TList, template<class> class Predicate>
using filter_t = typename filter<TList, Predicate>::type;

} //namespace

#endif
