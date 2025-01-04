//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_REGION_PROXY_HPP
#define MAKI_REGION_PROXY_HPP

#include "detail/region_proxy_fwd.hpp"
#include "detail/state_proxy_fwd.hpp"
#include <functional>

namespace maki
{

namespace detail
{
    template<class Region>
    region_proxy<Region> make_region_proxy(const Region& region)
    {
        return region_proxy<Region>{region};
    }
}

/**
@brief Provides read-only access to an internal region object.
*/
#ifdef MAKI_DETAIL_DOXYGEN
template<IMPLEMENTATION_DETAIL>
#else
template<class Region>
#endif
class region_proxy
{
public:
    region_proxy(const Region& region):
        region_(region)
    {
    }

    region_proxy(const region_proxy&) = default;
    region_proxy(region_proxy&&) = delete;
    region_proxy& operator=(const region_proxy&) = default;
    region_proxy& operator=(region_proxy&&) = delete;
    ~region_proxy() = default;

    [[nodiscard]] bool running() const
    {
        return region_.get().running();
    }

    template<const auto& StateConf>
    [[nodiscard]] bool is() const
    {
        return region_.get().template is<StateConf>();
    }

    template<const auto& StateConf>
    [[nodiscard]] auto state() const
    {
        return detail::make_state_proxy(region_.get().template state<StateConf>());
    }

private:
    std::reference_wrapper<const Region> region_;
};

} //namespace

#endif
