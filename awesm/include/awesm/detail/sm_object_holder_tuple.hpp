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
        template<class Sm, class Context>
        sm_object_holder_tuple(Sm& machine, Context& ctx):
            sm_object_holder_tuple<Ts...>(machine, ctx),
            obj_{machine, ctx}
        {
        }

        using sm_object_holder_tuple<Ts...>::get;

        T& get(T* /*tag*/)
        {
            return obj_.object;
        }

        const T& get(T* /*tag*/) const
        {
            return obj_.object;
        }

        template<int Index>
        auto& get()
        {
            if constexpr(Index == 0)
            {
                return obj_.object;
            }
            else
            {
                return sm_object_holder_tuple<Ts...>::template get<Index - 1>();
            }
        }

        template<int Index>
        const auto& get() const
        {
            if constexpr(Index == 0)
            {
                return obj_.object;
            }
            else
            {
                return sm_object_holder_tuple<Ts...>::template get<Index - 1>();
            }
        }

        template<class F>
        void for_each(F&& f)
        {
            f(obj_.object);
            sm_object_holder_tuple<Ts...>::for_each(f);
        }

        template<class F>
        void for_each(F&& f) const
        {
            f(obj_.object);
            sm_object_holder_tuple<Ts...>::for_each(f);
        }

    private:
        sm_object_holder<T> obj_;
};

template<>
class sm_object_holder_tuple<>
{
    public:
        template<class Sm, class Context>
        sm_object_holder_tuple(Sm& /*machine*/, Context& /*ctx*/)
        {
        }

        void get() const
        {
        }

        template<class F>
        void for_each(F&& /*f*/) const
        {
        }
};

} //namespace

#endif
