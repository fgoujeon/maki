//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_without_event.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_CALL_MEMBER_HPP
#define MAKI_DETAIL_CALL_MEMBER_HPP

#include "tlu.hpp"
#include "../action_signature.hpp"
#include "../guard_signature.hpp"
#include <functional>
#include <utility>

namespace maki::detail
{

template
<
    class Signature,
    Signature Sig,
    class Callable,
    class Context,
    class Machine,
    class Event,
    class... ExtraArgs
>
auto call_callable
(
    Callable& callable,
    [[maybe_unused]] Context& ctx,
    [[maybe_unused]] Machine& mach,
    [[maybe_unused]] const Event& event,
    [[maybe_unused]] ExtraArgs&&... extra_args
)
{
    if constexpr(Sig == Signature::v)
    {
        return std::invoke(callable, std::forward<ExtraArgs>(extra_args)...);
    }
    else if constexpr(Sig == Signature::c)
    {
        return std::invoke(callable, ctx, std::forward<ExtraArgs>(extra_args)...);
    }
    else if constexpr(Sig == Signature::cm)
    {
        return std::invoke(callable, ctx, mach, std::forward<ExtraArgs>(extra_args)...);
    }
    else if constexpr(Sig == Signature::cme)
    {
        return std::invoke(callable, ctx, mach, event, std::forward<ExtraArgs>(extra_args)...);
    }
    else if constexpr(Sig == Signature::ce)
    {
        return std::invoke(callable, ctx, event, std::forward<ExtraArgs>(extra_args)...);
    }
    else if constexpr(Sig == Signature::m)
    {
        return std::invoke(callable, mach, std::forward<ExtraArgs>(extra_args)...);
    }
    else if constexpr(Sig == Signature::me)
    {
        return std::invoke(callable, mach, event, std::forward<ExtraArgs>(extra_args)...);
    }
    else if constexpr(Sig == Signature::e)
    {
        return std::invoke(callable, event, std::forward<ExtraArgs>(extra_args)...);
    }
    else
    {
        constexpr auto is_false = sizeof(Machine) == 0;
        static_assert(is_false, "Unsupported signature");
    }
}

template
<
    class Action,
    class Context,
    class Machine,
    class Event
>
void call_action
(
    const Action& act,
    Context& ctx,
    Machine& mach,
    const Event& event
)
{
    return call_callable<action_signature, Action::signature>
    (
        act.callable,
        ctx,
        mach,
        event
    );
}

template
<
    class Guard,
    class Context,
    class Machine,
    class Event
>
bool call_guard
(
    const Guard& grd,
    Context& ctx,
    Machine& mach,
    const Event& event
)
{
    return call_callable<guard_signature, Guard::signature>
    (
        grd.callable,
        ctx,
        mach,
        event
    );
}

} //namespace

#endif
