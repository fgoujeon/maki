//Copyright Florian Goujeon 2021 - 2024.
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

#include "guard_signature.hpp"
#include "detail/signature_macros.hpp"
#include "detail/call.hpp"

namespace maki
{

template<guard_signature Sig, class Callable>
struct guard
{
    static constexpr guard_signature signature = Sig;
    Callable callable;
};

#define MAKI_DETAIL_X(name) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
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

} //namespace

#endif
