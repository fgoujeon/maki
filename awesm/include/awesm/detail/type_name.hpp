//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_TYPE_NAME_HPP
#define AWESM_DETAIL_TYPE_NAME_HPP

#include <string_view>

namespace awesm::detail
{

namespace type_name_detail
{
    template<class T>
    constexpr std::string_view function_name()
    {
#ifdef _MSC_VER
        return static_cast<const char*>(__FUNCSIG__);
#else
        return static_cast<const char*>(__PRETTY_FUNCTION__);
#endif
    }

    struct type_name_format
    {
        int prefix_size = 0;
        int suffix_size = 0;
    };

    constexpr type_name_format get_type_name_format()
    {
        constexpr auto int_name = std::string_view{"int"};
        constexpr auto int_function_name = function_name<int>();
        constexpr auto prefix_size = static_cast<int>(int_function_name.find(int_name));
        constexpr auto suffix_size = static_cast<int>(int_function_name.size() - prefix_size - int_name.size());
        return type_name_format
        {
            prefix_size,
            suffix_size
        };
    }

    constexpr auto format = get_type_name_format();

    template<class T>
    constexpr std::string_view get_type_name()
    {
        const auto fn_name = function_name<T>();
        return fn_name.substr
        (
            format.prefix_size,
            fn_name.size() - format.prefix_size - format.suffix_size
        );
    }
}

template<class T>
constexpr auto type_name = type_name_detail::get_type_name<T>();

} //namespace

#endif
