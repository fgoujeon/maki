//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_CALL_MEMBER_HPP
#define MAKI_DETAIL_CALL_MEMBER_HPP

#include "tlu.hpp"
#include <functional>
#include <utility>

namespace maki::detail
{

template
<
    bool Async,
    class R,
    class Callable,
    class... Args
>
R call_callable_2
(
    Callable& callable,
    Args&&... args
)
{
#if __cpp_impl_coroutine
    if constexpr(Async)
    {
        using callable_return_type = std::invoke_result_t<Callable, Args...>;

        if constexpr(std::is_same_v<callable_return_type, R>)
        {
            return std::invoke(callable, std::forward<Args>(args)...);
        }
        else
        {
            /*
            Wrap the callable into a coroutine so that we can return `R`
            in all cases.
            */
            return [&]() -> R
            {
                co_return std::invoke(callable, std::forward<Args>(args)...);
            }();
        }
    }
    else
#endif
    {
        if constexpr(std::is_void_v<R>)
        {
            std::invoke(callable, std::forward<Args>(args)...);
        }
        else
        {
            return std::invoke(callable, std::forward<Args>(args)...);
        }
    }
}

template
<
    bool Async,
    class R,
    class Signature,
    Signature Sig,
    class Callable,
    class Context,
    class Machine,
    class Event,
    class... ExtraArgs
>
R call_callable
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
        return call_callable_2<Async, R>(callable, std::forward<ExtraArgs>(extra_args)...);
    }
    else if constexpr(Sig == Signature::c)
    {
        return call_callable_2<Async, R>(callable, ctx, std::forward<ExtraArgs>(extra_args)...);
    }
    else if constexpr(Sig == Signature::cm)
    {
        return call_callable_2<Async, R>(callable, ctx, mach, std::forward<ExtraArgs>(extra_args)...);
    }
    else if constexpr(Sig == Signature::cme)
    {
        return call_callable_2<Async, R>(callable, ctx, mach, event, std::forward<ExtraArgs>(extra_args)...);
    }
    else if constexpr(Sig == Signature::ce)
    {
        return call_callable_2<Async, R>(callable, ctx, event, std::forward<ExtraArgs>(extra_args)...);
    }
    else if constexpr(Sig == Signature::m)
    {
        return call_callable_2<Async, R>(callable, mach, std::forward<ExtraArgs>(extra_args)...);
    }
    else if constexpr(Sig == Signature::me)
    {
        return call_callable_2<Async, R>(callable, mach, event, std::forward<ExtraArgs>(extra_args)...);
    }
    else if constexpr(Sig == Signature::e)
    {
        return call_callable_2<Async, R>(callable, event, std::forward<ExtraArgs>(extra_args)...);
    }
    else
    {
        constexpr auto is_false = sizeof(Machine) == 0;
        static_assert(is_false, "Unsupported signature");
    }
}

} //namespace

#endif
