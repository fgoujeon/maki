//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_REGION_PATH_HPP
#define AWESM_REGION_PATH_HPP

#include "transition_table.hpp"
#include "pretty_name.hpp"
#include "detail/sm_conf_traits.hpp"
#include "detail/tlu.hpp"
#include <string>

namespace awesm
{

template<class SmOrCompositeState, int RegionIndex>
struct region_path_element
{
    static_assert(RegionIndex >= 0);

    using sm_type = SmOrCompositeState;
    static constexpr auto region_index = RegionIndex;

    static std::string to_string()
    {
        auto str = std::string{};
        str += awesm::get_pretty_name<sm_type>();

        if constexpr(detail::sm_conf_traits::region_count_v<typename sm_type::conf> > 1)
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
        static_assert(sm_conf_traits::region_count_v<typename SmOrCompositeState::conf> == 1);
        using type = region_path<Ts..., region_path_element<SmOrCompositeState, 0>>;
    };
}

template<class... Ts>
struct region_path
{
    //RegionIndex MUST be specified for machines or composite states with several regions
    template<class SmOrCompositeState, int RegionIndex = -1>
    using add = typename detail::region_path_add<region_path, SmOrCompositeState, RegionIndex>::type;

    static std::string to_string()
    {
        if constexpr(sizeof...(Ts) == 0)
        {
            return "";
        }
        else
        {
            auto str = ((Ts::to_string() + ".") + ...);
            str.resize(str.size() - 1);
            return str;
        }
    }
};

//RegionIndex MUST be specified for machines with several regions
template<class Sm, int RegionIndex = -1>
using make_region_path = region_path<>::add<Sm, RegionIndex>;

} //namespace

#endif
