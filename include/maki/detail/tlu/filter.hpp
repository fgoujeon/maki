//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_FILTER_HPP
#define MAKI_DETAIL_TLU_FILTER_HPP

#include "push_front_if.hpp"

namespace maki::detail::tlu
{

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
    using type = push_front_if_t
    <
        typename filter<TList<Ts...>, Predicate>::type,
        T,
        Predicate<T>::value
    >;
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
