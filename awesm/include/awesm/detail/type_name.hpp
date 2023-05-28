//Copyright Florian Goujeon 2021 - 2023.
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
    using sv_size_t = std::string_view::size_type;

    template<class T>
    std::string_view function_name()
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

    inline type_name_format make_type_name_format()
    {
        const auto int_name = std::string_view{"int"};
        const auto int_function_name = function_name<int>();
        const auto prefix_size = static_cast<int>(int_function_name.find(int_name));
        const auto suffix_size =
            static_cast<int>(int_function_name.size()) -
            prefix_size -
            static_cast<int>(int_name.size())
        ;
        return type_name_format
        {
            prefix_size,
            suffix_size
        };
    }

    inline type_name_format cached_type_name_format()
    {
        static const auto format = make_type_name_format();
        return format;
    }

    template<class T>
    std::string_view type_name()
    {
        const auto& format = cached_type_name_format();
        const auto fn_name = function_name<T>();
        return fn_name.substr
        (
            static_cast<sv_size_t>(format.prefix_size),
            fn_name.size() - static_cast<sv_size_t>(format.prefix_size) - static_cast<sv_size_t>(format.suffix_size)
        );
    }

    //Extract "e" from e.g. a::b<c,d>::e<f::g>
    inline std::string_view decay_type_name(const std::string_view tname)
    {
        auto current_index = static_cast<int>(tname.size() - 1);

        //Find end index
        const auto end_index = [&]
        {
            auto template_level = 0;
            for(; current_index >= 0; --current_index)
            {
                switch(tname[static_cast<sv_size_t>(current_index)])
                {
                    case '<':
                        --template_level;
                        break;
                    case '>':
                        ++template_level;
                        break;
                    default:
                        if(template_level == 0)
                        {
                            return static_cast<sv_size_t>(current_index);
                        }
                }
            }
            return 0UL;
        }();

        //Find start index
        const auto start_index = [&]
        {
            for(; current_index >= 1; --current_index)
            {
                if(tname[static_cast<sv_size_t>(current_index - 1)] == ':')
                {
                    return static_cast<sv_size_t>(current_index);
                }
            }
            return 0UL;
        }();

        return tname.substr
        (
            start_index,
            end_index - start_index + 1
        );
    }
}

template<class T>
std::string_view type_name()
{
    static const auto name = type_name_detail::type_name<T>();
    return name;
}

template<class T>
std::string_view decayed_type_name()
{
    static const auto name = type_name_detail::decay_type_name
    (
        type_name_detail::type_name<T>()
    );
    return name;
}

} //namespace

#endif
