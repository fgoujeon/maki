//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STORABLE_FUNCTION_HPP
#define MAKI_DETAIL_STORABLE_FUNCTION_HPP

namespace maki::detail
{
    template<class R, class... Args>
    using function_ptr = R(*)(Args...);

    template<class R, class... Args>
    using function_type = R(Args...);

    /*
    Converts a function type into a function pointer type that can be stored as
    a struct member.
    */
    template<class F>
    struct storable_function
    {
        using type = F;
    };

    template<class R, class... Args>
    struct storable_function<function_type<R, Args...>>
    {
        using type = function_ptr<R, Args...>;
    };

    template<class F>
    using storable_function_t = typename storable_function<F>::type;
} //namespace

#endif
