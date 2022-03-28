//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_DETAIL_FOR_EACH_HPP
#define FGFSM_DETAIL_FOR_EACH_HPP

#include <tuple>

namespace fgfsm::detail
{

/*
for_each calls f(element) for each element of the given tuple.
*/

namespace for_each_detail
{
    template<class Tuple_>
    struct helper;

    template<class... Ts>
    struct helper<std::tuple<Ts...>>
    {
        template<class F, class Tuple_>
        static void call(F&& f, Tuple_&& t)
        {
            (f(std::get<Ts>(t)), ...);
        }
    };
}

template<class Tuple, class F>
void for_each(Tuple&& t, F&& f)
{
    using decaid_tuple = std::decay_t<Tuple>;
    for_each_detail::helper<decaid_tuple>::call
    (
        std::forward<F>(f),
        std::forward<Tuple>(t)
    );
}

} //namespace

#endif
