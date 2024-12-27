//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_without_event.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_CALL_MEMBER_HPP
#define MAKI_DETAIL_CALL_MEMBER_HPP

#include "tlu.hpp"
#include "../action.hpp"
#include "../guard_signature.hpp"
#include <functional>

namespace maki::detail
{

template<class Signature, Signature Sig, class Callable, class Context, class Machine, class Event>
auto invoke_action_or_guard_callable
(
    Callable& callable,
    [[maybe_unused]] Context& ctx,
    [[maybe_unused]] Machine& mach,
    [[maybe_unused]] const Event& event
)
{
    if constexpr(Sig == Signature::v)
    {
        return std::invoke(callable);
    }
    else if constexpr(Sig == Signature::c)
    {
        return std::invoke(callable, ctx);
    }
    else if constexpr(Sig == Signature::cm)
    {
        return std::invoke(callable, ctx, mach);
    }
    else if constexpr(Sig == Signature::cme)
    {
        return std::invoke(callable, ctx, mach, event);
    }
    else if constexpr(Sig == Signature::ce)
    {
        return std::invoke(callable, ctx, event);
    }
    else if constexpr(Sig == Signature::m)
    {
        return std::invoke(callable, mach);
    }
    else if constexpr(Sig == Signature::me)
    {
        return std::invoke(callable, mach, event);
    }
    else if constexpr(Sig == Signature::e)
    {
        return std::invoke(callable, event);
    }
    else
    {
        constexpr auto is_false = sizeof(Machine) == 0;
        static_assert(is_false, "Unsupported signature");
    }
}

template
<
    action_signature Signature,
    class Callable,
    class Context,
    class Machine,
    class Event
>
void call_action
(
    const action<Signature, Callable>& act,
    Context& ctx,
    Machine& mach,
    const Event& event
)
{
    return invoke_action_or_guard_callable<action_signature, Signature>
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
    return invoke_action_or_guard_callable<guard_signature, Guard::signature>
    (
        grd.callable,
        ctx,
        mach,
        event
    );
}

} //namespace

#endif
