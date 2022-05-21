//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_SM_OBJECT_HOLDER_TUPLE_HPP
#define AWESM_DETAIL_SM_OBJECT_HOLDER_TUPLE_HPP

#include "sm_object_holder.hpp"

namespace awesm::detail
{

/*
A minimal std::tuple-like container for SM objects.
Using this instead of std::tuple improves build time.
*/

template<class... Ts>
class sm_object_holder_tuple;

template<class T, class... Ts>
class sm_object_holder_tuple<T, Ts...>: public sm_object_holder_tuple<Ts...>
{
    public:
        template<class Context, class Sm>
        sm_object_holder_tuple(Context& ctx, Sm& machine):
            sm_object_holder_tuple<Ts...>(ctx, machine),
            obj_{ctx, machine}
        {
        }

        using sm_object_holder_tuple<Ts...>::get;

        T& get(T*)
        {
            return obj_.object;
        }

        const T& get(T*) const
        {
            return obj_.object;
        }

    private:
        sm_object_holder<T> obj_;
};

template<>
class sm_object_holder_tuple<>
{
    public:
        template<class Context, class Sm>
        sm_object_holder_tuple(Context&, Sm&)
        {
        }

        void get() const
        {
        }
};

} //namespace

#endif
