//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_without_event.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_CALL_MEMBER_HPP
#define MAKI_DETAIL_CALL_MEMBER_HPP

#include "type_traits.hpp"
#include "state_traits.hpp"
#include "tlu.hpp"
#include "../filters.hpp"
#include <functional>
#include <type_traits>
#include <utility>

namespace maki::detail
{

template<class Fn, class Machine, class Context, class Event>
auto call_action_or_guard
(
    const Fn& fun,
    [[maybe_unused]] Machine& mach,
    [[maybe_unused]] Context& ctx,
    [[maybe_unused]] const Event& event
)
{
    if constexpr(std::is_invocable_v<Fn, Context&, Machine&, const Event&>)
    {
        return std::invoke(fun, ctx, mach, event);
    }
    else if constexpr(std::is_invocable_v<Fn, Context&, const Event&>)
    {
        return std::invoke(fun, ctx, event);
    }
    else if constexpr(std::is_invocable_v<Fn, Context&>)
    {
        return std::invoke(fun, ctx);
    }
    else if constexpr(is_nullary_v<Fn>)
    {
        return std::invoke(fun);
    }
    else
    {
        constexpr auto is_false = sizeof(Machine) == 0;
        static_assert(is_false, "No valid signature found for action/guard");
    }
}

} //namespace

#endif
