//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_REGION_HPP
#define MAKI_REGION_HPP

#include "detail/friendly_impl.hpp"
#include <utility>

namespace maki
{

/**
@brief Represents an [region](@ref region)
@tparam Impl implementation detail

Objects of this type can only be created by Maki itself.
*/
template<class Impl>
class region
{
public:
#ifndef MAKI_DETAIL_DOXYGEN
    template<class... Args>
    region(Args&&... args):
        impl_(this, std::forward<Args>(args)...)
    {
    }
#endif

    region(const region&) = delete;
    region(region&&) = delete;
    region& operator=(const region&) = delete;
    region& operator=(region&&) = delete;
    ~region() = default;

    /**
    @brief Returns whether the state created by `StateMold` is active.
    */
    template<const auto& StateMold>
    [[nodiscard]] bool is() const
    {
        return impl_.template is<StateMold>();
    }

    /**
    @brief Returns the `maki::state` object created by `StateMold` (of type
    `maki::state_mold`).
    */
    template<const auto& StateMold>
    [[nodiscard]] const auto& state() const
    {
        return impl_.template state<StateMold>();
    }

    /**
    @brief Returns the path to the region.
    */
#ifdef MAKI_DETAIL_DOXYGEN
    static constexpr maki::path<IMPLEMENTATION_DETAIL> path()
#else
    static constexpr auto path()
#endif
    {
        return Impl::path();
    }

private:
    MAKI_DETAIL_FRIENDLY_IMPL

    using impl_type = Impl;

    impl_type impl_;
};

} //namespace

#endif
