//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_DETAIL_FIRST_ARG_HPP
#define FGFSM_DETAIL_FIRST_ARG_HPP

namespace fgfsm::detail
{

//first_arg gets the type of the first parameter of the given function pointer.

namespace first_arg_detail
{
    template<class F>
    struct impl;

    template<class R, class Arg, class... Args>
    struct impl<R(*)(Arg, Args...)>
    {
        using type = Arg;
    };
}

template<class F>
using first_arg = typename first_arg_detail::impl<F>::type;

} //namespace

#endif
