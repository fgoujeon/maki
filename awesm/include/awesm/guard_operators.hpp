//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_GUARD_OPERATORS_HPP
#define AWESM_GUARD_OPERATORS_HPP

#include "detail/sm_object.hpp"
#include "detail/call_member.hpp"

namespace awesm
{

namespace detail
{
    struct and_
    {
        static bool test(const bool l, const bool r)
        {
            return l && r;
        }
    };

    struct or_
    {
        static bool test(const bool l, const bool r)
        {
            return l || r;
        }
    };

    struct xor_
    {
        static bool test(const bool l, const bool r)
        {
            return l != r;
        }
    };

    template<class Lhs, class Rhs, class Operator>
    class binary_operator_guard
    {
        public:
            template<class Context, class Sm>
            binary_operator_guard(Context& context, Sm& machine)
            {
                detail::make_sm_object_if_null<Lhs>(context, machine);
                detail::make_sm_object_if_null<Rhs>(context, machine);
            }

            binary_operator_guard(const binary_operator_guard&) = delete;
            binary_operator_guard(binary_operator_guard&&) = delete;

            ~binary_operator_guard()
            {
                detail::unmake_sm_object_if_not_null<Lhs>();
                detail::unmake_sm_object_if_not_null<Rhs>();
            }

            binary_operator_guard& operator=(const binary_operator_guard&) = delete;
            binary_operator_guard& operator=(binary_operator_guard&&) = delete;

            template<class SourceState, class Event, class TargetState>
            bool check
            (
                SourceState& source_state,
                const Event& event,
                TargetState& target_state
            )
            {
                return Operator::test
                (
                    detail::call_check
                    (
                        &detail::get_sm_object<Lhs>(),
                        &source_state,
                        &event,
                        &target_state
                    ),
                    detail::call_check
                    (
                        &detail::get_sm_object<Rhs>(),
                        &source_state,
                        &event,
                        &target_state
                    )
                );
            }
    };
}

template<class Guard>
class not_
{
    public:
        template<class Context, class Sm>
        not_(Context& context, Sm& machine)
        {
            detail::make_sm_object_if_null<Guard>(context, machine);
        }

        not_(const not_&) = delete;
        not_(not_&&) = delete;

        ~not_()
        {
            detail::unmake_sm_object_if_not_null<Guard>();
        }

        not_& operator=(const not_&) = delete;
        not_& operator=(not_&&) = delete;

        template<class SourceState, class Event, class TargetState>
        bool check
        (
            SourceState& source_state,
            const Event& event,
            TargetState& target_state
        )
        {
            return !detail::call_check
            (
                &detail::get_sm_object<Guard>(),
                &source_state,
                &event,
                &target_state
            );
        }
};

template<class L, class R>
using and_ = detail::binary_operator_guard<L, R, detail::and_>;

template<class L, class R>
using or_ = detail::binary_operator_guard<L, R, detail::or_>;

template<class L, class R>
using xor_ = detail::binary_operator_guard<L, R, detail::xor_>;

} //namespace

#endif
