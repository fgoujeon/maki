//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_GUARD_OPERATORS_HPP
#define FGFSM_GUARD_OPERATORS_HPP

#include "detail/call_member.hpp"

namespace fgfsm
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
            template<class Context>
            binary_operator_guard(Context& context):
                lhs_{context},
                rhs_{context}
            {
            }

            template<class Event>
            bool check(const Event& event)
            {
                return Operator::test
                (
                    detail::call_check(lhs_, event),
                    detail::call_check(rhs_, event)
                );
            }

        private:
            Lhs lhs_;
            Rhs rhs_;
    };
}

template<class T>
class not_
{
    public:
        template<class Context>
        not_(Context& context):
            guard_{context}
        {
        }

        template<class Event>
        bool check(const Event& event)
        {
            return !detail::call_check(guard_, event);
        }

    private:
        T guard_;
};

template<class L, class R>
using and_ = detail::binary_operator_guard<L, R, detail::and_>;

template<class L, class R>
using or_ = detail::binary_operator_guard<L, R, detail::or_>;

template<class L, class R>
using xor_ = detail::binary_operator_guard<L, R, detail::xor_>;

} //namespace

#endif
