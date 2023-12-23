//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_REGION_PATH_OF_HPP
#define MAKI_DETAIL_REGION_PATH_OF_HPP

#include "submachine_fwd.hpp"
#include "region_fwd.hpp"
#include "../region_path.hpp"

namespace maki::detail
{

template<class T>
struct region_path_of;

template<class T>
inline constexpr auto region_path_of_v = region_path_of<T>::value;

template<const auto& Conf, class ParentRegion>
struct region_path_of<submachine<Conf, ParentRegion>>
{
    static constexpr auto value = region_path_of_v<ParentRegion>;
};

template<const auto& Conf>
struct region_path_of<submachine<Conf, void>>
{
    static constexpr auto value = region_path{};
};

template<class ParentSm, int Index>
struct region_path_of<region<ParentSm, Index>>
{
    static constexpr auto value = region_path_of_v<ParentSm>.add(ParentSm::conf, Index);
};

} //namespace

#endif
