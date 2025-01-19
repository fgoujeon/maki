//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::guard class template
*/

#ifndef MAKI_GUARD_HPP
#define MAKI_GUARD_HPP

#include "detail/signature_macros.hpp"
#include "detail/call.hpp"
#include "null.hpp"

namespace maki
{

/**
@brief The set of arguments taken by a guard callable.
*/
enum class guard_signature: char
{
    ///`bool guard()`
    v,

    ///`bool guard(context&)`
    c,

    ///`bool guard(context&, machine&)`
    cm,

    ///`bool guard(context&, machine&, const event&)`
    cme,

    ///`bool guard(context&, const event&)`
    ce,

    ///`bool guard(machine&)`
    m,

    ///`bool guard(machine&, const event&)`
    me,

    ///`bool guard(const event&)`
    e,
};

namespace detail
{
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
}

/**
@brief Represents a guard to be given to `maki::transition_table`.
Use the builder functions (`maki::guard_v()` and its variants) instead of using
this type directly.
*/
template<guard_signature Sig, class Callable>
struct guard
{
    static constexpr guard_signature signature = Sig;
    Callable callable;
};

#define MAKI_DETAIL_X(name) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    /** \
    @brief Makes a `maki::guard` with the indicated signature and given  \
    callable. \
    */ \
    template<class Callable> \
    constexpr guard<guard_signature::name, Callable> guard_##name(const Callable& callable) \
    { \
        return {callable}; \
    }
MAKI_DETAIL_GUARD_SIGNATURES
#undef MAKI_DETAIL_X

template
<
    guard_signature LhsSignature, class LhsCallable,
    guard_signature RhsSignature, class RhsCallable
>
constexpr auto operator&&
(
    const guard<LhsSignature, LhsCallable>& lhs,
    const guard<RhsSignature, RhsCallable>& rhs
)
{
    return guard_cme
    (
        [lhs, rhs](auto& ctx, auto& mach, const auto& event)
        {
            return
                detail::call_guard(lhs, ctx, mach, event) &&
                detail::call_guard(rhs, ctx, mach, event)
            ;
        }
    );
}

template
<
    guard_signature LhsSignature, class LhsCallable,
    guard_signature RhsSignature, class RhsCallable
>
constexpr auto operator||
(
    const guard<LhsSignature, LhsCallable>& lhs,
    const guard<RhsSignature, RhsCallable>& rhs
)
{
    return guard_cme
    (
        [lhs, rhs](auto& ctx, auto& mach, const auto& event)
        {
            return
                detail::call_guard(lhs, ctx, mach, event) ||
                detail::call_guard(rhs, ctx, mach, event)
            ;
        }
    );
}

template
<
    guard_signature LhsSignature, class LhsCallable,
    guard_signature RhsSignature, class RhsCallable
>
constexpr auto operator!=
(
    const guard<LhsSignature, LhsCallable>& lhs,
    const guard<RhsSignature, RhsCallable>& rhs
)
{
    return guard_cme
    (
        [lhs, rhs](auto& ctx, auto& mach, const auto& event)
        {
            return
                detail::call_guard(lhs, ctx, mach, event) !=
                detail::call_guard(rhs, ctx, mach, event)
            ;
        }
    );
}

template<guard_signature Signature, class Callable>
constexpr auto operator!(const guard<Signature, Callable>& grd)
{
    return guard_cme
    (
        [grd](auto& ctx, auto& mach, const auto& event)
        {
            return !detail::call_guard(grd, ctx, mach, event);
        }
    );
}

namespace detail
{
    inline constexpr auto null_guard = guard_v([]{return true;});

    template<guard_signature Sig, class Callable>
    constexpr const auto& to_guard(const guard<Sig, Callable>& grd)
    {
        return grd;
    }

    constexpr const auto& to_guard(null_t /*ignored*/)
    {
        return null_guard;
    }

    template<class T>
    struct is_guard
    {
        static constexpr auto value = false;
    };

    template<guard_signature Sig, class Callable>
    struct is_guard<guard<Sig, Callable>>
    {
        static constexpr auto value = true;
    };

    template<class T>
    constexpr bool is_guard_v = is_guard<T>::value;
}

} //namespace

#endif
