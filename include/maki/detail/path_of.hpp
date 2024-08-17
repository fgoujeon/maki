//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_PATH_OF_HPP
#define MAKI_DETAIL_PATH_OF_HPP

#include "submachine_fwd.hpp"
#include "submachine_no_context_fwd.hpp"
#include "region_fwd.hpp"
#include "../path.hpp"

namespace maki::detail
{

template<class T>
struct path_of;

template<class T>
inline constexpr auto path_of_v = path_of<T>::value;

template<auto Id, class ParentRegion>
struct path_of<submachine<Id, ParentRegion>>
{
    static constexpr auto value = path_of_v<ParentRegion> / *Id;
};

template<auto Id>
struct path_of<submachine<Id, void>>
{
    static constexpr auto value = path{*Id};
};

template<auto Id, class ParentRegion>
struct path_of<submachine_no_context<Id, ParentRegion>>
{
    static constexpr auto value = path_of_v<ParentRegion> / *Id;
};

template<auto Id>
struct path_of<submachine_no_context<Id, void>>
{
    static constexpr auto value = path{*Id};
};

template<class ParentSm, int Index>
struct path_of<region<ParentSm, Index>>
{
    static constexpr auto value = path_of_v<ParentSm> / Index;
};

} //namespace

#endif
