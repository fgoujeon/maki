//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_REGION_PROXY_HPP
#define MAKI_REGION_PROXY_HPP

#include "detail/state_proxy_fwd.hpp"
#include <utility>

namespace maki
{

/**
@brief Provides read-only access to an internal region object.
*/
#ifdef MAKI_DETAIL_DOXYGEN
template<IMPLEMENTATION_DETAIL>
#else
template<class Impl>
#endif
class region_proxy
{
public:
    template<class... Args>
    region_proxy(Args&&... args):
        impl_(std::forward<Args>(args)...)
    {
    }

    region_proxy(const region_proxy&) = default;
    region_proxy(region_proxy&&) = delete;
    region_proxy& operator=(const region_proxy&) = default;
    region_proxy& operator=(region_proxy&&) = delete;
    ~region_proxy() = default;

    [[nodiscard]] bool running() const
    {
        return impl_.running();
    }

    template<const auto& StateConf>
    [[nodiscard]] bool is() const
    {
        return impl_.template is<StateConf>();
    }

    template<const auto& StateConf>
    [[nodiscard]] auto state() const
    {
        return detail::make_state_proxy(impl_.template state<StateConf>());
    }

#ifndef MAKI_DETAIL_DOXYGEN
    Impl& impl()
    {
        return impl_;
    }

    const Impl& impl() const
    {
        return impl_;
    }
#endif

private:
    Impl impl_;
};

} //namespace

#endif
