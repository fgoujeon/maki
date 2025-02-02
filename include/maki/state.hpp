//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_REGION_STATE_HPP
#define MAKI_REGION_STATE_HPP

#include "detail/pretty_name.hpp"
#include "detail/impl.hpp"
#include <string_view>
#include <utility>

namespace maki
{

/**
@brief Represents a [state](@ref state).
@tparam Impl implementation detail

Objects of this type can only be created by Maki itself.
*/
template<class Impl>
class state
{
public:
#ifndef MAKI_DETAIL_DOXYGEN
    template<class... Args>
    constexpr state(Args&&... args):
        impl_(std::forward<Args>(args)...)
    {
    }
#endif

    state(const state&) = delete;
    state(state&&) = delete;
    state& operator=(const state&) = delete;
    state& operator=(state&&) = delete;
    ~state() = default;

    /**
    @brief Returns the `maki::state` object created from `StateConf` (of type
    `maki::state_conf`). Only valid if state is composite and only made of one
    region.
    */
    template<const auto& StateConf>
    [[nodiscard]] const auto& substate() const
    {
        return impl_.template state<StateConf>();
    }

    /**
    @brief Returns the `maki::region` object at index `Index`. Only valid if
    state is composite.
    */
    template<int Index>
    [[nodiscard]] const auto& region() const
    {
        return impl_.template region<Index>();
    }

    /**
    @brief Returns whether the state created from `StateConf` is active. Only
    valid if state is composite and only made of one region.
    */
    template<const auto& StateConf>
    [[nodiscard]] bool is() const
    {
        return impl_.template is<StateConf>();
    }

    /**
    @brief Returns the context instantiated at construction.
    */
    [[nodiscard]] const auto& context() const
    {
        return impl_.context();
    }

    /**
    @brief The pretty name of the state.

    Returns either:
    - the value given to `maki::state_conf::pretty_name()`, if any;
    - the name of the `maki::state_conf` variable, without scope nor template
    argument list.
    */
    [[nodiscard]] static std::string_view pretty_name()
    {
        return detail::pretty_name<Impl::conf>();
    }

private:
    using impl_type = Impl;

    MAKI_DETAIL_FRIENDLY_IMPL
};

} //namespace

#endif
