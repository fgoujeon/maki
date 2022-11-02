//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_REGION_PATH_HPP
#define AWESM_REGION_PATH_HPP

namespace awesm
{

template<class SmOrCompositeState, int RegionIndex>
struct region_path_element
{
    using sm_t = SmOrCompositeState;
    static constexpr auto region_index = RegionIndex;
};

template<class... Ts>
struct region_path{};

} //namespace

#endif
