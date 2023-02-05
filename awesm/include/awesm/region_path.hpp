//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_REGION_PATH_HPP
#define AWESM_REGION_PATH_HPP

#include "transition_table.hpp"
#include "pretty_name.hpp"
#include "detail/sm_traits.hpp"
#include <string>

namespace awesm
{

template<class SmOrCompositeState, int RegionIndex>
struct region_path_element
{
    static_assert(RegionIndex >= 0);

    using sm_type = SmOrCompositeState;
    static constexpr auto region_index = RegionIndex;

    static std::string get_pretty_name()
    {
        auto str = std::string{};
        str += awesm::get_pretty_name<sm_type>();

        if constexpr(detail::sm_traits::region_count_v<sm_type> > 1)
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
        static_assert(sm_traits::region_count_v<SmOrCompositeState> == 1);
        using type = region_path<Ts..., region_path_element<SmOrCompositeState, 0>>;
    };
}

template<class... Ts>
struct region_path
{
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

//RegionIndex MUST be specified for machines with several regions
template<class Sm, int RegionIndex = -1>
using make_region_path = region_path<>::add<Sm, RegionIndex>;

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
