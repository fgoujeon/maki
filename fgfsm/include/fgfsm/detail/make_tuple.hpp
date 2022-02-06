//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_DETAIL_MAKE_TUPLE_HPP
#define FGFSM_DETAIL_MAKE_TUPLE_HPP

#include <tuple>
#include <utility>

namespace fgfsm::detail
{

/*
make_tuple creates a std::tuple whose all elements are instanciated with the
same given reference.

In this example...:
    int data = 42;
    using my_tuple = std::tuple<type0, type1>;
    auto t = make_tuple<my_tuple>(data);

... the tuple elements are instanciated as type0{data} and type1{data}.
*/

namespace make_tuple_detail
{
    template<class Tuple>
    struct helper;

    template<class... Ts>
    struct helper<std::tuple<Ts...>>
    {
        template<class Arg>
        static auto make(Arg& arg)
        {
            return std::tuple<Ts...>{Ts{arg}...};
        }
    };
}

template<class Tuple, class Arg>
auto make_tuple(Arg& arg)
{
    return make_tuple_detail::helper<Tuple>::make(arg);
}

} //namespace

#endif
