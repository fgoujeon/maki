//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_GUARD_HPP
#define AWESM_GUARD_HPP

#include "detail/call_member.hpp"
#include "detail/alternative.hpp"

namespace awesm
{

namespace detail
{
    //Variables can't have function types. Only pointer to functions are valid
    //variable types.
    template<class F>
    using storable_function_t = detail::alternative_t
    <
        std::is_function_v<F>,
        F&,
        F
    >;

#define AWESM_GUARD_OPERATOR(op_name, op) \
    template<class Lhs, class Rhs> \
    class op_name##_t \
    { \
        public: \
            constexpr op_name##_t(const Lhs& lhs, const Rhs& rhs): \
                lhs_(lhs), \
                rhs_(rhs) \
            { \
            } \
    \
            template<class Sm, class Context, class Event> \
            bool operator()(Sm& mach, Context& ctx, const Event& event) const \
            { \
                return \
                    call_action_or_guard(lhs_, &mach, ctx, &event) op \
                    call_action_or_guard(rhs_, &mach, ctx, &event) \
                ; \
            } \
    \
        private: \
            storable_function_t<Lhs> lhs_; \
            storable_function_t<Rhs> rhs_; \
    };

    AWESM_GUARD_OPERATOR(and, &&)
    AWESM_GUARD_OPERATOR(or, ||)
    AWESM_GUARD_OPERATOR(xor, !=)

#undef AWESM_GUARD_OPERATOR

    template<class Guard>
    class not_t
    {
        public:
            constexpr not_t(const Guard& grd):
                grd_(grd)
            {
            }

            template<class Sm, class Context, class Event>
            bool operator()(Sm& mach, Context& ctx, const Event& event) const
            {
                return !call_action_or_guard(grd_, &mach, ctx, &event);
            }

        private:
            storable_function_t<Guard> grd_;
    };

    template<class Guard>
    not_t(const Guard&) -> not_t<Guard>;
}

template<class F>
class guard_t
{
    public:
        constexpr guard_t(const F& fun):
            f_(fun)
        {
        }

        template<class F2>
        constexpr guard_t(const guard_t<F2>& grd):
            f_(grd.f_)
        {
        }

        template<class Sm, class Context, class Event>
        bool operator()(Sm& mach, Context& ctx, const Event& event) const
        {
            return call_action_or_guard(f_, &mach, ctx, &event);
        }

        template<class Rhs>
        constexpr auto operator&&(const guard_t<Rhs>& rhs) const
        {
            using type = detail::and_t<F, Rhs>;
            return guard_t<type>{type{f_, rhs.f_}};
        }

        template<class Rhs>
        constexpr auto operator||(const guard_t<Rhs>& rhs) const
        {
            using type = detail::or_t<F, Rhs>;
            return guard_t<type>{type{f_, rhs.f_}};
        }

        template<class Rhs>
        constexpr auto operator!=(const guard_t<Rhs>& rhs) const
        {
            using type = detail::xor_t<F, Rhs>;
            return guard_t<type>{type{f_, rhs.f_}};
        }

        constexpr auto operator!() const
        {
            using type = detail::not_t<F>;
            return guard_t<type>{type{f_}};
        }

    private:
        detail::storable_function_t<F> f_;
};

template<class F>
guard_t(const F&) -> guard_t<F>;

} //namespace

#endif
