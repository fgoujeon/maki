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

    private:
        template<class T2, class... T2s>
        friend T2& get(sm_object_holder_tuple<T2s...>&);

        template<class T2, class... T2s>
        friend const T2& get(const sm_object_holder_tuple<T2s...>&);
};

template<class T, class... Ts>
T& get(sm_object_holder_tuple<Ts...>& tuple)
{
    return static_cast<sm_object_holder<T>&>(tuple);
}

template<class T, class... Ts>
const T& get(const sm_object_holder_tuple<Ts...>& tuple)
{
    return static_cast<const sm_object_holder<T>&>(tuple);
}

template<int Index, class... Ts>
auto& get(sm_object_holder_tuple<Ts...>& tuple)
{
    using sm_object_holder_type_list_t = type_list<sm_object_holder<Ts>...>;
    using sm_object_holder_t = tlu::at_t<sm_object_holder_type_list_t, Index>;
    using object_t = typename sm_object_holder_t::object_t;
    return get<object_t>(tuple);
}

template<int Index, class... Ts>
const auto& get(const sm_object_holder_tuple<Ts...>& tuple)
{
    using sm_object_holder_type_list_t = type_list<sm_object_holder<Ts>...>;
    using sm_object_holder_t = tlu::at_t<sm_object_holder_type_list_t, Index>;
    using object_t = typename sm_object_holder_t::object_type;
    return get<object_t>(tuple);
}

} //namespace

#endif
