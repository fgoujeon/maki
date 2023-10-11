//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_GUARD_HPP
#define MAKI_GUARD_HPP

#include "detail/call_member.hpp"
#include <type_traits>

namespace maki
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
struct guard;

template<const auto& Guard>
inline constexpr auto guard_c = guard<detail::guard_operator::none, Guard, Guard>{};

template<detail::guard_operator Operator, const auto& Operand, const auto& Operand2>
inline constexpr auto composable_guard_c = guard<Operator, Operand, Operand2>{};

template<detail::guard_operator Operator, const auto& Operand, const auto& Operand2>
struct guard
{
    template<class Sm, class Context, class Event>
    bool operator()(Sm& mach, Context& ctx, const Event& event) const
    {
        if constexpr(Operator == detail::guard_operator::none)
        {
            return detail::call_action_or_guard<Operand>(mach, ctx, event);
        }
        if constexpr(Operator == detail::guard_operator::not_)
        {
            return !Operand(mach, ctx, event);
        }
        if constexpr(Operator == detail::guard_operator::and_)
        {
            return Operand(mach, ctx, event) && Operand2(mach, ctx, event);
        }
        if constexpr(Operator == detail::guard_operator::or_)
        {
            return Operand(mach, ctx, event) || Operand2(mach, ctx, event);
        }
        if constexpr(Operator == detail::guard_operator::xor_)
        {
            return Operand(mach, ctx, event) != Operand2(mach, ctx, event);
        }
    }

    template
    <
        detail::guard_operator RhsOperator, const auto& RhsOperand, const auto& RhsOperand2
    >
    constexpr const auto& operator&&
    (
        guard<RhsOperator, RhsOperand, RhsOperand2> /*rhs*/
    ) const
    {
        return composable_guard_c
        <
            detail::guard_operator::and_,
            composable_guard_c<Operator, Operand, Operand2>,
            composable_guard_c<RhsOperator, RhsOperand, RhsOperand2>
        >;
    }

    template
    <
        detail::guard_operator RhsOperator, const auto& RhsOperand, const auto& RhsOperand2
    >
    constexpr const auto& operator||
    (
        guard<RhsOperator, RhsOperand, RhsOperand2> /*rhs*/
    ) const
    {
        return composable_guard_c
        <
            detail::guard_operator::or_,
            composable_guard_c<Operator, Operand, Operand2>,
            composable_guard_c<RhsOperator, RhsOperand, RhsOperand2>
        >;
    }

    template
    <
        detail::guard_operator RhsOperator, const auto& RhsOperand, const auto& RhsOperand2
    >
    constexpr const auto& operator!=
    (
        guard<RhsOperator, RhsOperand, RhsOperand2> /*rhs*/
    ) const
    {
        return composable_guard_c
        <
            detail::guard_operator::xor_,
            composable_guard_c<Operator, Operand, Operand2>,
            composable_guard_c<RhsOperator, RhsOperand, RhsOperand2>
        >;
    }

    constexpr const auto& operator!() const
    {
        return composable_guard_c
        <
            detail::guard_operator::not_,
            composable_guard_c<Operator, Operand, Operand2>,
            composable_guard_c<Operator, Operand, Operand2>
        >;
    }
};

} //namespace

#endif
