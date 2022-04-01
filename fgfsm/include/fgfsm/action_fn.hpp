//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_ACTION_FN_HPP
#define FGFSM_ACTION_FN_HPP

#include "any_cref.hpp"
#include "detail/function_traits.hpp"

namespace fgfsm
{

/*
An adapter that wraps a pointer to a function of the following form:
    void f(context& ctx, const event_type& event)
    {
        //...
    }
into a struct of the following form:
    struct s
    {
        void execute(const fgfsm::any_cref& event)
        {
            fgfsm::visit
            (
                event,
                [this](const event_type& event)
                {
                    //...
                }
            );
        }

        context& ctx;
    };
or, if event_type is fgfsm::any_cref:
    struct s
    {
        void execute(const fgfsm::any_cref& event)
        {
            //...
        }

        context& ctx;
    };
*/
template<auto F>
class action_fn
{
    private:
        using context_arg_t = detail::function_traits::first_arg<decltype(F)>;
        using event_arg_t = detail::function_traits::second_arg<decltype(F)>;
        using event_t = std::decay_t<event_arg_t>;

    public:
        action_fn(context_arg_t ctx):
            ctx_(ctx)
        {
        }

        void execute(const any_cref& event)
        {
            if constexpr(std::is_same_v<event_t, any_cref>)
            {
                F(ctx_, event);
            }
            else
            {
                visit
                (
                    event,
                    [this](event_arg_t event)
                    {
                        F(ctx_, event);
                    }
                );
            }
        }

    private:
        context_arg_t ctx_;
};

} //namespace

#endif
