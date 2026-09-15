//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

namespace maki::detail
{

#if MAKI_AOS_ASYNC
template
<
    class AsyncResult,
    class Callable,
    class... Args
>
AsyncResult async_call_callable_2
(
    Callable& callable,
    Args&&... args
)
{
    using callable_return_type = std::invoke_result_t<Callable, Args...>;

    if constexpr(std::is_same_v<callable_return_type, AsyncResult>)
    {
        return std::invoke(callable, std::forward<Args>(args)...);
    }
    else
    {
        /*
        Wrap the callable into a coroutine so that we can return `AsyncResult`
        in all cases.
        */
        return [&]() -> AsyncResult
        {
            co_return std::invoke(callable, std::forward<Args>(args)...);
        }();
    }
}
#else
template
<
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
    if constexpr(std::is_void_v<R>)
    {
        std::invoke(callable, std::forward<Args>(args)...);
    }
    else
    {
        return std::invoke(callable, std::forward<Args>(args)...);
    }
}
#endif

template
<
    class R,
    class Signature,
    Signature Sig,
    class Callable,
    class Context,
    class Machine,
    class Event,
    class... ExtraArgs
>
R MAKI_AOS(call_callable)
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
        return MAKI_AOS(call_callable_2)<R>(callable, std::forward<ExtraArgs>(extra_args)...);
    }
    else if constexpr(Sig == Signature::c)
    {
        return MAKI_AOS(call_callable_2)<R>(callable, ctx, std::forward<ExtraArgs>(extra_args)...);
    }
    else if constexpr(Sig == Signature::cm)
    {
        return MAKI_AOS(call_callable_2)<R>(callable, ctx, mach, std::forward<ExtraArgs>(extra_args)...);
    }
    else if constexpr(Sig == Signature::cme)
    {
        return MAKI_AOS(call_callable_2)<R>(callable, ctx, mach, event, std::forward<ExtraArgs>(extra_args)...);
    }
    else if constexpr(Sig == Signature::ce)
    {
        return MAKI_AOS(call_callable_2)<R>(callable, ctx, event, std::forward<ExtraArgs>(extra_args)...);
    }
    else if constexpr(Sig == Signature::m)
    {
        return MAKI_AOS(call_callable_2)<R>(callable, mach, std::forward<ExtraArgs>(extra_args)...);
    }
    else if constexpr(Sig == Signature::me)
    {
        return MAKI_AOS(call_callable_2)<R>(callable, mach, event, std::forward<ExtraArgs>(extra_args)...);
    }
    else if constexpr(Sig == Signature::e)
    {
        return MAKI_AOS(call_callable_2)<R>(callable, event, std::forward<ExtraArgs>(extra_args)...);
    }
    else
    {
        constexpr auto is_false = sizeof(Machine) == 0;
        static_assert(is_false, "Unsupported signature");
    }
}

} //namespace
