//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_REGION_PATH_OF_HPP
#define MAKI_DETAIL_REGION_PATH_OF_HPP

#include "submachine_fwd.hpp"
#include "region_fwd.hpp"

namespace maki::detail
{

//Must be specialized by each type
template<class T>
struct region_path_of;

template<class T>
inline constexpr auto region_path_of_v = region_path_of<T>::value;

template<class ConfHolder, class ParentRegion>
struct region_path_of<submachine<ConfHolder, ParentRegion>>
{
    static constexpr auto value = region_path_of_v<ParentRegion>;
};

template<class ConfHolder>
struct region_path_of<submachine<ConfHolder, void>>
{
    static constexpr auto value = region_path{};
};

template<class ParentSm, int Index>
struct region_path_of<region<ParentSm, Index>>
{
    static constexpr auto value = region_path_of_v<ParentSm>.template add<ParentSm::conf_holder_type::conf, Index>();
};

//Must be specialized by each type
template<class T>
struct machine_of;

template<class T>
using root_sm_of_t = typename machine_of<T>::type;

} //namespace

#endif
