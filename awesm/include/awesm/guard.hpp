//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_GUARD_HPP
#define AWESM_GUARD_HPP

#include "detail/call_member.hpp"
#include "detail/alternative.hpp"
#include <type_traits>

namespace awesm
{

namespace detail
{
    //Variables can't have function types, we must use a reference.
    template<class F>
    using storable_function_t = detail::alternative_t
    <
        std::is_function_v<F>,
        const F&,
        F
    >;

    template<auto Operator, class Lhs, class Rhs>
    class binary_operator
    {
        public:
            constexpr binary_operator(const Lhs& lhs, const Rhs& rhs):
                lhs_(lhs),
                rhs_(rhs)
            {
            }

            template<class Sm, class Context, class Event>
            bool operator()(Sm& mach, Context& ctx, const Event& event) const
            {
                return Operator
                (
                    call_action_or_guard(lhs_, &mach, ctx, &event),
                    call_action_or_guard(rhs_, &mach, ctx, &event)
                );
            }

        private:
            storable_function_t<Lhs> lhs_;
            storable_function_t<Rhs> rhs_;
    };

    inline bool and_(const bool lhs, const bool rhs)
    {
        return lhs && rhs;
    }

    inline bool or_(const bool lhs, const bool rhs)
    {
        return lhs || rhs;
    }

    inline bool xor_(const bool lhs, const bool rhs)
    {
        return lhs != rhs;
    }

    template<class Lhs, class Rhs>
    constexpr auto make_and_operator(const Lhs& lhs, const Rhs& rhs)
    {
        return binary_operator<and_, Lhs, Rhs>(lhs, rhs);
    }

    template<class Lhs, class Rhs>
    constexpr auto make_or_operator(const Lhs& lhs, const Rhs& rhs)
    {
        return binary_operator<or_, Lhs, Rhs>(lhs, rhs);
    }

    template<class Lhs, class Rhs>
    constexpr auto make_xor_operator(const Lhs& lhs, const Rhs& rhs)
    {
        return binary_operator<xor_, Lhs, Rhs>{lhs, rhs};
    }

    template<class Guard>
    class not_t
    {
        public:
            explicit constexpr not_t(const Guard& grd):
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

template<class Guard>
class guard_t
{
    public:
        explicit constexpr guard_t(const Guard& grd):
            grd_(grd)
        {
        }

        template<class Guard2>
        constexpr guard_t(const guard_t<Guard2>& grd):
            grd_(grd.grd_)
        {
        }

        template<class Sm, class Context, class Event>
        bool operator()(Sm& mach, Context& ctx, const Event& event) const
        {
            return detail::call_action_or_guard(grd_, &mach, ctx, &event);
        }

        constexpr const Guard& get() const
        {
            return grd_;
        }

    private:
        detail::storable_function_t<Guard> grd_;
};

template<class F>
guard_t(const F&) -> guard_t<F>;

template<class Lhs, class Rhs>
constexpr auto operator&&(const guard_t<Lhs>& lhs, const guard_t<Rhs>& rhs)
{
    return guard_t{detail::make_and_operator(lhs.get(), rhs.get())};
}

template<class Lhs, class Rhs>
constexpr auto operator&&(const guard_t<Lhs>& lhs, const Rhs& rhs)
{
    return guard_t{detail::make_and_operator(lhs.get(), rhs)};
}

template<class Lhs, class Rhs>
constexpr auto operator&&(const Lhs& lhs, const guard_t<Rhs>& rhs)
{
    return guard_t{detail::make_and_operator(lhs, rhs.get())};
}

template<class Lhs, class Rhs>
constexpr auto operator||(const guard_t<Lhs>& lhs, const guard_t<Rhs>& rhs)
{
    return guard_t{detail::make_or_operator(lhs.get(), rhs.get())};
}

template<class Lhs, class Rhs>
constexpr auto operator||(const guard_t<Lhs>& lhs, const Rhs& rhs)
{
    return guard_t{detail::make_or_operator(lhs.get(), rhs)};
}

template<class Lhs, class Rhs>
constexpr auto operator||(const Lhs& lhs, const guard_t<Rhs>& rhs)
{
    return guard_t{detail::make_or_operator(lhs, rhs.get())};
}

template<class Lhs, class Rhs>
constexpr auto operator!=(const guard_t<Lhs>& lhs, const guard_t<Rhs>& rhs)
{
    return guard_t{detail::make_xor_operator(lhs.get(), rhs.get())};
}

template<class Lhs, class Rhs>
constexpr auto operator!=(const guard_t<Lhs>& lhs, const Rhs& rhs)
{
    return guard_t{detail::make_xor_operator(lhs.get(), rhs)};
}

template<class Lhs, class Rhs>
constexpr auto operator!=(const Lhs& lhs, const guard_t<Rhs>& rhs)
{
    return guard_t{detail::make_xor_operator(lhs, rhs.get())};
}

template<class Guard>
constexpr auto operator!(const guard_t<Guard>& grd)
{
    return guard_t{detail::not_t{grd.get()}};
}

template<const auto& Guard>
inline constexpr auto guard = guard_t{Guard};

} //namespace

#endif
