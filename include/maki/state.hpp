//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_REGION_STATE_HPP
#define MAKI_REGION_STATE_HPP

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
class state
{
public:
    using impl_type = Impl;

    template<class... Args>
    state(Args&&... args):
        impl_(std::forward<Args>(args)...)
    {
    }

    state(const state&) = default;
    state(state&&) = delete;
    state& operator=(const state&) = default;
    state& operator=(state&&) = delete;
    ~state() = default;

    template<const auto& StateConf>
    [[nodiscard]] const auto& substate() const
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
