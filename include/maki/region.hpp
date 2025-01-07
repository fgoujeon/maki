//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_REGION_HPP
#define MAKI_REGION_HPP

#include <utility>

namespace maki
{

/**
@brief Represents an [orthogonal region](@ref orthogonal-region)
*/
#ifdef MAKI_DETAIL_DOXYGEN
template<IMPLEMENTATION_DETAIL>
#else
template<class Impl>
#endif
class region
{
public:
    template<class... Args>
    region(Args&&... args):
        impl_(std::forward<Args>(args)...)
    {
    }

    region(const region&) = default;
    region(region&&) = delete;
    region& operator=(const region&) = default;
    region& operator=(region&&) = delete;
    ~region() = default;

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
    [[nodiscard]] const auto& state() const
    {
        return impl_.template state<StateConf>();
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
