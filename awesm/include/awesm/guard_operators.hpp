//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_GUARD_OPERATORS_HPP
#define AWESM_GUARD_OPERATORS_HPP

#include "detail/call_member.hpp"

namespace awesm
{

namespace detail
{
    template<const auto&... Guards>
    struct and_t
    {
        template<class Sm, class Context, class Event>
        bool operator()(Sm& mach, Context& /*ctx*/, const Event& event) const
        {
            return (call_action_or_guard(Guards, mach, &event) && ...);
        }
    };

    template<const auto&... Guards>
    struct or_t
    {
        template<class Sm, class Context, class Event>
        bool operator()(Sm& mach, Context& /*ctx*/, const Event& event) const
        {
            return (call_action_or_guard(Guards, mach, &event) || ...);
        }
    };

    template<const auto&... Guards>
    struct xor_t
    {
        template<class Sm, class Context, class Event>
        bool operator()(Sm& mach, Context& /*ctx*/, const Event& event) const
        {
            return (call_action_or_guard(Guards, mach, &event) != ...);
        }
    };

    template<const auto& Guard>
    struct not_t
    {
        template<class Sm, class Context, class Event>
        bool operator()(Sm& mach, Context& /*ctx*/, const Event& event) const
        {
            return !call_action_or_guard(Guard, mach, &event);
        }
    };
}

template<const auto&... Guards>
constexpr inline auto and_ = detail::and_t<Guards...>{};

template<const auto&... Guards>
constexpr inline auto or_ = detail::or_t<Guards...>{};

template<const auto&... Guards>
constexpr inline auto xor_ = detail::xor_t<Guards...>{};

template<const auto& Guard>
constexpr inline auto not_ = detail::not_t<Guard>{};

} //namespace

#endif
