//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_FUNCTION_TRAITS_HPP
#define AWESM_DETAIL_FUNCTION_TRAITS_HPP

namespace awesm::detail::function_traits
{

namespace first_arg_detail
{
    template<class F>
    struct impl;

    template<class R, class Arg0, class... Args>
    struct impl<R(*)(Arg0, Args...)>
    {
        using type = Arg0;
    };
}

namespace second_arg_detail
{
    template<class F>
    struct impl;

    template<class R, class Arg0, class Arg1, class... Args>
    struct impl<R(*)(Arg0, Arg1, Args...)>
    {
        using type = Arg1;
    };
}

//Gets the type of the first parameter of the given function pointer.
template<class F>
using first_arg = typename first_arg_detail::impl<F>::type;

//Gets the type of the second parameter of the given function pointer.
template<class F>
using second_arg = typename second_arg_detail::impl<F>::type;

} //namespace

#endif
