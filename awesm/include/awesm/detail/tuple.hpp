//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_TUPLE_HPP
#define AWESM_DETAIL_TUPLE_HPP

#include "sm_object_holder.hpp"
#include "type_list.hpp"
#include "tlu.hpp"

namespace awesm::detail
{

/*
A minimal std::tuple-like container.
Using this instead of std::tuple improves build time.
*/

template<class... Ts>
class tuple: private Ts...
{
    public:
        template<class Arg>
        explicit tuple(Arg& arg):
            Ts(arg)...
        {
        }

    private:
        template<class T2, class... T2s>
        friend T2& get(tuple<T2s...>&);

        template<class T2, class... T2s>
        friend const T2& get(const tuple<T2s...>&);

        template<int Index, class... T2s>
        friend auto& get(tuple<T2s...>&);

        template<int Index, class... T2s>
        friend const auto& get(const tuple<T2s...>&);
};

template<class T, class... Ts>
T& get(tuple<Ts...>& tuple)
{
    return static_cast<T&>(tuple);
}

template<class T, class... Ts>
const T& get(const tuple<Ts...>& tuple)
{
    return static_cast<const T&>(tuple);
}

template<int Index, class... Ts>
auto& get(tuple<Ts...>& tuple)
{
    using type_list_t = type_list<Ts...>;
    using type_t = tlu::at_t<type_list_t, Index>;
    return get<type_t>(tuple);
}

template<int Index, class... Ts>
const auto& get(const tuple<Ts...>& tuple)
{
    using type_list_t = type_list<Ts...>;
    using type_t = tlu::at_t<type_list_t, Index>;
    return get<type_t>(tuple);
}

template<class F, class... Ts>
void for_each(tuple<Ts...>& tuple, F&& callback)
{
    (callback(get<Ts>(tuple)), ...);
}

template<class F, class... Ts>
void for_each(const tuple<Ts...>& tuple, F&& callback)
{
    (callback(get<Ts>(tuple)), ...);
}

} //namespace

#endif
