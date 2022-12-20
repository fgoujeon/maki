//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_TLU_FILTER_HPP
#define AWESM_DETAIL_TLU_FILTER_HPP

namespace awesm::detail::tlu
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
        template<class...> class List,
        class... Ts,
        class... Us
    >
    struct concat<List<Ts...>, List<Us...>>
    {
        using type = List<Ts..., Us...>;
    };

    template
    <
        template<class...> class List,
        class T,
        bool B
    >
    struct make_type_list_from_type_if
    {
        using type = List<T>;
    };

    template
    <
        template<class...> class List,
        class T
    >
    struct make_type_list_from_type_if<List, T, false>
    {
        using type = List<>;
    };

    template
    <
        template<class> class Predicate,
        class TList
    >
    struct filter_list;

    template
    <
        template<class> class Predicate,
        template<class...> class List,
        class T,
        class... Ts
    >
    struct filter_list<Predicate, List<T, Ts...>>
    {
        using type = typename concat
        <
            typename make_type_list_from_type_if<List, T, Predicate<T>::value>::type,
            typename filter_list<Predicate, List<Ts...>>::type
        >::type;
    };

    template
    <
        template<class> class Predicate,
        template<class...> class List
    >
    struct filter_list<Predicate, List<>>
    {
        using type = List<>;
    };
}

/*
filter takes a type list and a predicate, and returns a new type list whose
each type T verifies Predicate<T>::value == true.
*/
template<class TList, template<class> class Predicate>
using filter = typename filter_detail::filter_list<Predicate, TList>::type;

} //namespace

#endif
