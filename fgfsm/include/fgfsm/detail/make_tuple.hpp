//Copyright Florian Goujeon 2021.
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
make_tuple creates a std::tuple whose items are uniformly instanciated.

In this example...:
    using my_tuple = std::tuple<type0, type1>;
    auto t = make_tuple<my_tuple>{0, 'a'};

... the tuple elements are instanciated as type0{0, 'a'} and type1{0, 'a'}.
*/

namespace make_tuple_detail
{
    template<class Tuple>
    struct helper;

    template<class... Ts>
    struct helper<std::tuple<Ts...>>
    {
        template<class... Args>
        static auto make(Args&&... args)
        {
            return std::tuple<Ts...>
            {
                Ts{std::forward<Args>(args)...}...
            };
        }
    };
}

template<class Tuple, class... Args>
auto make_tuple(Args&&... args)
{
    return make_tuple_detail::helper<Tuple>::make(std::forward<Args>(args)...);
}

} //namespace

#endif
