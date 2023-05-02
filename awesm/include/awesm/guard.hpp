//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_GUARD_HPP
#define AWESM_GUARD_HPP

#include "detail/call_member.hpp"
#include <type_traits>

namespace awesm
{

namespace detail
{
    enum class guard_operator
    {
        none,
        not_,
        and_,
        or_,
        xor_
    };
}

template<detail::guard_operator Operator, const auto& Operand, const auto& Operand2>
class guard_t
{
public:
    template<class Sm, class Context, class Event>
    bool operator()(Sm& mach, Context& ctx, const Event& event) const
    {
        if constexpr(Operator == detail::guard_operator::none)
        {
            return detail::call_action_or_guard<Operand>(mach, ctx, event);
        }
        if constexpr(Operator == detail::guard_operator::not_)
        {
            return !detail::call_action_or_guard<Operand>(mach, ctx, event);
        }
        if constexpr(Operator == detail::guard_operator::and_)
        {
            return
                detail::call_action_or_guard<Operand>(mach, ctx, event) &&
                detail::call_action_or_guard<Operand2>(mach, ctx, event)
            ;
        }
        if constexpr(Operator == detail::guard_operator::or_)
        {
            return
                detail::call_action_or_guard<Operand>(mach, ctx, event) ||
                detail::call_action_or_guard<Operand2>(mach, ctx, event)
            ;
        }
        if constexpr(Operator == detail::guard_operator::xor_)
        {
            return
                detail::call_action_or_guard<Operand>(mach, ctx, event) !=
                detail::call_action_or_guard<Operand2>(mach, ctx, event)
            ;
        }
    }
};

template<const auto& Guard>
inline constexpr auto guard = guard_t<detail::guard_operator::none, Guard, Guard>{};

template<detail::guard_operator Operator, const auto& Operand, const auto& Operand2>
inline constexpr auto composable_guard = guard_t<Operator, Operand, Operand2>{};

template
<
    detail::guard_operator LhsOperator, const auto& LhsOperand, const auto& LhsOperand2,
    detail::guard_operator RhsOperator, const auto& RhsOperand, const auto& RhsOperand2
>
constexpr const auto& operator&&
(
    const guard_t<LhsOperator, LhsOperand, LhsOperand2>& /*lhs*/,
    const guard_t<RhsOperator, RhsOperand, RhsOperand2>& /*rhs*/
)
{
    return composable_guard
    <
        detail::guard_operator::and_,
        composable_guard<LhsOperator, LhsOperand, LhsOperand2>,
        composable_guard<RhsOperator, RhsOperand, RhsOperand2>
    >;
}

template
<
    detail::guard_operator LhsOperator, const auto& LhsOperand, const auto& LhsOperand2,
    detail::guard_operator RhsOperator, const auto& RhsOperand, const auto& RhsOperand2
>
constexpr const auto& operator||
(
    const guard_t<LhsOperator, LhsOperand, LhsOperand2>& /*lhs*/,
    const guard_t<RhsOperator, RhsOperand, RhsOperand2>& /*rhs*/
)
{
    return composable_guard
    <
        detail::guard_operator::or_,
        composable_guard<LhsOperator, LhsOperand, LhsOperand2>,
        composable_guard<RhsOperator, RhsOperand, RhsOperand2>
    >;
}

template
<
    detail::guard_operator LhsOperator, const auto& LhsOperand, const auto& LhsOperand2,
    detail::guard_operator RhsOperator, const auto& RhsOperand, const auto& RhsOperand2
>
constexpr const auto& operator!=
(
    const guard_t<LhsOperator, LhsOperand, LhsOperand2>& /*lhs*/,
    const guard_t<RhsOperator, RhsOperand, RhsOperand2>& /*rhs*/
)
{
    return composable_guard
    <
        detail::guard_operator::xor_,
        composable_guard<LhsOperator, LhsOperand, LhsOperand2>,
        composable_guard<RhsOperator, RhsOperand, RhsOperand2>
    >;
}

template
<
    detail::guard_operator Operator, const auto& Operand, const auto& Operand2
>
constexpr const auto& operator!
(
    const guard_t<Operator, Operand, Operand2>& /*grd*/
)
{
    return composable_guard
    <
        detail::guard_operator::not_,
        composable_guard<Operator, Operand, Operand2>,
        composable_guard<Operator, Operand, Operand2>
    >;
}

} //namespace

#endif
