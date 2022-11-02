//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_SM_PATH_HPP
#define AWESM_DETAIL_SM_PATH_HPP

#include "../region_path.hpp"
#include "tlu.hpp"

namespace awesm::detail
{

template<class RegionPath, class SmOrCompositeState>
struct sm_path
{
    using region_path_t = RegionPath;
    using sm_t = SmOrCompositeState;
};

template<class SmPath, int RegionIndex>
struct make_region_path
{
    using type = tlu::push_back
    <
        typename SmPath::region_path_t,
        region_path_element
        <
            typename SmPath::sm_t,
            RegionIndex
        >
    >;
};

template<class SmPath, int RegionIndex>
using make_region_path_t =
    typename make_region_path<SmPath, RegionIndex>::type
;

} //namespace

#endif