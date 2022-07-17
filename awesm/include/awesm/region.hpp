//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_REGION_HPP
#define AWESM_REGION_HPP

#include "detail/region_impl.hpp"

namespace awesm
{

template<class TransitionTableHolder>
class region:
    private detail::region_impl
    <
        region<TransitionTableHolder>,
        TransitionTableHolder
    >
{
    private:
        template<class RegionConfListHolder2>
        friend class subsm;

        template<class Derived, class RegionConfListHolder2>
        friend class detail::region_impl;

        using detail::region_impl<region, TransitionTableHolder>::region_impl;
};

} //namespace

#endif
