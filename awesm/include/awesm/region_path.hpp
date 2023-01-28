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
struct region_path;

namespace detail
{
    template<class RegionPathElement>
    struct is_valid_region_path_element
    {
        using conf_type = typename RegionPathElement::conf;
        using transition_table_type_list = detail::tlu::at_t<conf_type, 0>;
        static constexpr auto region_count = detail::tlu::size_v<transition_table_type_list>;
        static constexpr auto value = region_count() == 1;
    };

    template<class SmOrCompositeState, int RegionIndex>
    struct is_valid_region_path_element<region_path_element<SmOrCompositeState, RegionIndex>>
    {
        static constexpr auto value = true;
    };

    template<class RegionPathElement>
    inline constexpr auto is_valid_region_path_element_v =
        is_valid_region_path_element<RegionPathElement>::value
    ;

    template<class RegionPath, class SmOrCompositeState, int RegionIndex>
    struct region_path_add;

    template<class... Ts, class SmOrCompositeState, int RegionIndex>
    struct region_path_add<region_path<Ts...>, SmOrCompositeState, RegionIndex>
    {
        using type = region_path<Ts..., region_path_element<SmOrCompositeState, RegionIndex>>;
    };

    template<class... Ts, class SmOrCompositeState>
    struct region_path_add<region_path<Ts...>, SmOrCompositeState, -1>
    {
        using type = region_path<Ts..., SmOrCompositeState>;
    };
}

template<class... Ts>
struct region_path
{
    static_assert((detail::is_valid_region_path_element_v<Ts> && ...));

    //RegionIndex MUST be specified for machines or composite states with several regions
    template<class SmOrCompositeState, int RegionIndex = -1>
    using add = typename detail::region_path_add<region_path, SmOrCompositeState, RegionIndex>::type;

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

    template<class T>
    struct to_region_path_element
    {
        using type = region_path_element<T, 0>;
    };

    template<class SmOrCompositeState, int RegionIndex>
    struct to_region_path_element<region_path_element<SmOrCompositeState, RegionIndex>>
    {
        using type = region_path_element<SmOrCompositeState, RegionIndex>;
    };

    template<class T>
    using to_region_path_element_t = typename to_region_path_element<T>::type;

    template<class RegionPath>
    using region_path_front_element_t = to_region_path_element_t<tlu::at_t<RegionPath, 0>>;
}

} //namespace

#endif
