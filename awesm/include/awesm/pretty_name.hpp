//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_PRETTY_NAME_HPP
#define AWESM_PRETTY_NAME_HPP

#include "sm_conf.hpp"
#include "state_conf.hpp"
#include "detail/type_name.hpp"
#include "detail/tlu.hpp"
#include "detail/overload_priority.hpp"
#include <string_view>

namespace awesm
{

namespace detail
{
    struct get_pretty_name_option{};

    template<class T>
    decltype(auto) get_pretty_name_impl(overload_priority::low /*unused*/)
    {
        return get_decayed_type_name<T>();
    }

    template
    <
        class T,
        std::enable_if_t<!T::conf::is_composite, bool> = true,
        std::enable_if_t
        <
            tlu::at_f_t<typename T::conf, state_option::get_pretty_name>::value,
            bool
        > = true
    >
    static decltype(auto) get_pretty_name_impl(overload_priority::high /*unused*/)
    {
        return T::get_pretty_name();
    }

    template
    <
        class T,
        std::enable_if_t<T::conf::is_composite, bool> = true,
        std::enable_if_t
        <
            tlu::at_f_t<typename T::conf, sm_option::get_pretty_name>::value,
            bool
        > = true
    >
    static decltype(auto) get_pretty_name_impl(overload_priority::high /*unused*/)
    {
        return T::get_pretty_name();
    }
}

template<class T>
decltype(auto) get_pretty_name()
{
    return detail::get_pretty_name_impl<T>(detail::overload_priority::probe);
}

} //namespace

#endif
