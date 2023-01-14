//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_REGION_PATH_HPP
#define AWESM_REGION_PATH_HPP

#include "pretty_name.hpp"
#include <string>

namespace awesm
{

template<class SmOrCompositeState, int RegionIndex>
struct region_path_element
{
    using sm_type = SmOrCompositeState;
    static constexpr auto region_index = RegionIndex;

    static std::string get_pretty_name()
    {
        auto str = std::string{};
        str += awesm::get_pretty_name<sm_type>();

        if constexpr(sm_type::conf_type::region_count > 1)
        {
            str += "[";
            str += std::to_string(region_index);
            str += "]";
        }

        return str;
    }
};

template<class... Ts>
struct region_path
{
    static std::string get_pretty_name()
    {
        if constexpr(sizeof...(Ts) == 0)
        {
            return "";
        }
        else
        {
            auto str = ((Ts::get_pretty_name() + ".") + ...);
            str.resize(str.size() - 1);
            return str;
        }
    }
};

namespace detail
{
    template<class RegionPath>
    struct region_path_to_sm;

    template<class Elem, class... Elems>
    struct region_path_to_sm<region_path<Elem, Elems...>>
    {
        using type = typename Elem::sm_type;
    };

    template<class RegionPath>
    using region_path_to_sm_t = typename region_path_to_sm<RegionPath>::type;
}

} //namespace

#endif
