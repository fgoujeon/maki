//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_REGION_PROXY_HPP
#define MAKI_REGION_PROXY_HPP

#include "state_proxy.hpp"

namespace maki
{

template<class Region>
class region_proxy
{
public:
    region_proxy(const Region& region):
        region_(region)
    {
    }

    [[nodiscard]] bool running() const
    {
        return region_.running();
    }

    template<const auto& StateConf>
    [[nodiscard]] bool is() const
    {
        return region_.template is<StateConf>();
    }

    template<const auto& StateConf>
    [[nodiscard]] auto state() const
    {
        return state_proxy{region_.template state<StateConf>()};
    }

private:
    const Region& region_; //NOLINT cppcoreguidelines-avoid-const-or-ref-data-members
};

template<class Region>
region_proxy(const Region&) -> region_proxy<Region>;

} //namespace

#endif
