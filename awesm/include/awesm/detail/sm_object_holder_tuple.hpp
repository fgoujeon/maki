//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_SM_OBJECT_HOLDER_TUPLE_HPP
#define AWESM_DETAIL_SM_OBJECT_HOLDER_TUPLE_HPP

#include "sm_object_holder.hpp"
#include "type_list.hpp"
#include "tlu.hpp"

namespace awesm::detail
{

/*
A minimal std::tuple-like container for SM objects.
Using this instead of std::tuple improves build time.
*/

template<class... Ts>
class sm_object_holder_tuple: private sm_object_holder<Ts>...
{
    public:
        template<class Sm, class Context>
        sm_object_holder_tuple(Sm& machine, Context& ctx):
            sm_object_holder<Ts>(machine, ctx)...
        {
        }

        template<class T>
        T& get()
        {
            return sm_object_holder<T>::object;
        }

        template<class T>
        const T& get() const
        {
            return sm_object_holder<T>::object;
        }

        template<int Index>
        auto& get()
        {
            using sm_object_holder_type_list_t = type_list<sm_object_holder<Ts>...>;
            using sm_object_holder_t = tlu::at<sm_object_holder_type_list_t, Index>;
            using object_t = typename sm_object_holder_t::object_t;
            return get<object_t>();
        }

        template<int Index>
        const auto& get() const
        {
            using sm_object_holder_type_list_t = type_list<sm_object_holder<Ts>...>;
            using sm_object_holder_t = tlu::at<sm_object_holder_type_list_t, Index>;
            using object_t = typename sm_object_holder_t::object_t;
            return get<object_t>();
        }

        template<class F>
        void for_each(F&& callback)
        {
            (callback(get<Ts>()), ...);
        }

        template<class F>
        void for_each(F&& callback) const
        {
            (callback(get<Ts>()), ...);
        }
};

} //namespace

#endif
