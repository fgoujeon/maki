//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_REGION_STATE_PROXY_HPP
#define MAKI_REGION_STATE_PROXY_HPP

#include <utility>

namespace maki
{

/**
@brief Represents a [state](@ref state).
*/
#ifdef MAKI_DETAIL_DOXYGEN
template<IMPLEMENTATION_DETAIL>
#else
template<class Impl>
#endif
class state_proxy
{
public:
    using impl_type = Impl;

    template<class... Args>
    state_proxy(Args&&... args):
        impl_(std::forward<Args>(args)...)
    {
    }

    state_proxy(const state_proxy&) = default;
    state_proxy(state_proxy&&) = delete;
    state_proxy& operator=(const state_proxy&) = default;
    state_proxy& operator=(state_proxy&&) = delete;
    ~state_proxy() = default;

    template<const auto& StateConf>
    [[nodiscard]] const auto& state() const
    {
        return impl_.template state<StateConf>();
    }

    template<int Index>
    [[nodiscard]] const auto& region() const
    {
        return impl_.template region<Index>();
    }

    template<const auto& StateConf>
    [[nodiscard]] bool is() const
    {
        return impl_.template is<StateConf>();
    }

    [[nodiscard]] bool running() const
    {
        return impl_.running();
    }

    [[nodiscard]] const auto& context() const
    {
        return impl_.context();
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
