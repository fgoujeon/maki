//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_DETAIL_FSM_OBJECT_HOLDER_TUPLE_HPP
#define FGFSM_DETAIL_FSM_OBJECT_HOLDER_TUPLE_HPP

#include "fsm_object_holder.hpp"

namespace fgfsm::detail
{

/*
A minimal std::tuple-like container for FSM objects.
Using this instead of std::tuple improves build time.
*/

template<class... Ts>
class fsm_object_holder_tuple;

template<class T, class... Ts>
class fsm_object_holder_tuple<T, Ts...>: public fsm_object_holder_tuple<Ts...>
{
    public:
        template<class Context>
        fsm_object_holder_tuple(Context& ctx):
            fsm_object_holder_tuple<Ts...>(ctx),
            obj_{ctx}
        {
        }

        using fsm_object_holder_tuple<Ts...>::get;

        T& get(T*)
        {
            return obj_.object;
        }

        const T& get(T*) const
        {
            return obj_.object;
        }

    private:
        fsm_object_holder<T> obj_;
};

template<>
class fsm_object_holder_tuple<>
{
    public:
        template<class Context>
        fsm_object_holder_tuple(Context&)
        {
        }

        void get() const
        {
        }
};

} //namespace

#endif
