//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_GUARD_FN_HPP
#define FGFSM_GUARD_FN_HPP

#include "detail/function_traits.hpp"

namespace fgfsm
{

/*
An adapter that wraps a pointer to a function of the following form:
    bool f(context& ctx, const event_type& event)
    {
        //...
    }
into a struct of the following form:
    struct s
    {
        bool check(const event_type& event)
        {
            //...
        }

        context& ctx;
    };
*/
template<auto F>
class guard_fn
{
    private:
        using context_arg_t = detail::function_traits::first_arg<decltype(F)>;

    public:
        guard_fn(context_arg_t ctx):
            ctx_(ctx)
        {
        }

        template<class Event>
        bool check(const Event& event)
        {
            return F(ctx_, event);
        }

    private:
        context_arg_t ctx_;
};

} //namespace

#endif
