//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_GUARD_OPERATORS_HPP
#define AWESM_GUARD_OPERATORS_HPP

#include "detail/sm_object_holder_tuple.hpp"
#include "detail/sm_object_holder.hpp"
#include "detail/call_member.hpp"

namespace awesm
{

template<const auto&... Guards>
constexpr auto and_ = [](auto& mach, auto& ctx, const auto& event)
{
    return (detail::call_guard<Guards>(&mach, &ctx, &event) && ...);
};

template<const auto&... Guards>
constexpr auto or_ = [](auto& mach, auto& ctx, const auto& event)
{
    return (detail::call_guard<Guards>(&mach, &ctx, &event) || ...);
};

template<const auto&... Guards>
constexpr auto xor_ = [](auto& mach, auto& ctx, const auto& event)
{
    return (detail::call_guard<Guards>(&mach, &ctx, &event) != ...);
};

template<const auto& Guard>
constexpr auto not_ = [](auto& mach, auto& ctx, const auto& event)
{
    return !detail::call_guard<Guard>(&mach, &ctx, &event);
};

} //namespace

#endif
