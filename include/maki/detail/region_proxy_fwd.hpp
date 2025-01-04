//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_REGION_PROXY_FWD_HPP
#define MAKI_DETAIL_REGION_PROXY_FWD_HPP

namespace maki
{

template<class Region>
class region_proxy;

namespace detail
{
    template<class Region>
    region_proxy<Region> make_region_proxy(const Region& region);
}

} //namespace

#endif
