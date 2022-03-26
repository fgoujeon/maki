//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_FN_HPP
#define FGFSM_FN_HPP

#include "detail/first_arg.hpp"

namespace fgfsm
{

/*
An adapter that turns a pointer to a function of the following form:
    return_type f(context& ctx, other_arg_types...)
    {
        //...
    }
into a struct of the following form:
    struct s
    {
        return_type operator()(other_arg_types...)
        {
            //...
        }

        context& ctx;
    };

This allows to write actions and guards in a less verbose fashion.
*/
template<auto F>
struct fn
{
    template<class... Args>
    auto operator()(Args&&... args) const
    {
        return F(ctx, std::forward<Args>(args)...);
    }

    detail::first_arg<decltype(F)> ctx;
};

} //namespace

#endif
