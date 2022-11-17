//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_PRETTY_NAME_HPP
#define AWESM_PRETTY_NAME_HPP

#include "detail/type_name.hpp"
#include "detail/tlu.hpp"
#include <string_view>

namespace awesm
{

namespace detail
{
    struct get_pretty_name_option{};

    template<class T, class Enable = void>
    struct pretty_name_getter
    {
        static decltype(auto) get()
        {
            return get_decayed_type_name<T>();
        }
    };

    template<class T>
    struct pretty_name_getter
    <
        T,
        std::enable_if_t<tlu::contains<typename T::conf, get_pretty_name_option>>
    >
    {
        static decltype(auto) get()
        {
            return T::get_pretty_name();
        }
    };
}

template<class T>
decltype(auto) get_pretty_name()
{
    return detail::pretty_name_getter<T>::get();
}

} //namespace

#endif
