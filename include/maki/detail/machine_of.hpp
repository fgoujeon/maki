//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_MACHINE_OF_HPP
#define MAKI_DETAIL_MACHINE_OF_HPP

#include "submachine_fwd.hpp"
#include "region_fwd.hpp"
#include "../machine_fwd.hpp"

namespace maki::detail
{

template<class T>
struct machine_of;

template<class T>
using machine_of_t = typename machine_of<T>::type;

template<class ParentSm, int Index>
struct machine_of<region<ParentSm, Index>>
{
    using type = machine_of_t<ParentSm>;
};

template<class ConfHolder, class ParentRegion>
struct machine_of<submachine<ConfHolder, ParentRegion>>
{
    using type = machine_of_t<ParentRegion>;

    static type& get(submachine<ConfHolder, ParentRegion>& node)
    {
        return node.root_sm();
    }
};

template<class ConfHolder>
struct machine_of<submachine<ConfHolder, void>>
{
    using type = machine<ConfHolder>;

    static type& get(submachine<ConfHolder, void>& node)
    {
        return node.root_sm();
    }
};

} //namespace

#endif
