//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_GUARD_HPP
#define AWESM_GUARD_HPP

#include "detail/alternative.hpp"
#include <type_traits>

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
            template<class... Args> \
            auto operator()(Args&... args) const -> decltype(std::declval<Lhs>()(args...) op std::declval<Rhs>()(args...)) \
            { \
                return lhs_(args...) op rhs_(args...); \
            } \
    \
        private: \
            storable_function_t<Lhs> lhs_; \
            storable_function_t<Rhs> rhs_; \
    }; \
 \
    template<class Lhs, class Rhs> \
    op_name##_t(const Lhs&, const Rhs&) -> op_name##_t<Lhs, Rhs>;

    AWESM_GUARD_OPERATOR(and, &&)
    AWESM_GUARD_OPERATOR(or, ||)
    AWESM_GUARD_OPERATOR(xor, !=)

#undef AWESM_GUARD_OPERATOR

    template<class Guard>
    class not_t
    {
        public:
            explicit constexpr not_t(const Guard& grd):
                grd_(grd)
            {
            }

            template<class... Args>
            auto operator()(Args&... args) const -> decltype(!std::declval<Guard>()(args...))
            {
                return !grd_(args...);
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

        template<class... Args>
        auto operator()(Args&... args) const -> decltype(std::declval<Guard>()(args...))
        {
            return grd_(args...);
        }

    private:
        detail::storable_function_t<Guard> grd_;
};

template<class F>
guard_t(const F&) -> guard_t<F>;

template<class Lhs, class Rhs>
constexpr auto operator&&(const guard_t<Lhs>& lhs, const guard_t<Rhs>& rhs)
{
    return guard_t{detail::and_t{lhs, rhs}};
}

template<class Lhs, class Rhs>
constexpr auto operator&&(const guard_t<Lhs>& lhs, const Rhs& rhs)
{
    return guard_t{detail::and_t{lhs, rhs}};
}

template<class Lhs, class Rhs>
constexpr auto operator&&(const Lhs& lhs, const guard_t<Rhs>& rhs)
{
    return guard_t{detail::and_t{lhs, rhs}};
}

template<class Lhs, class Rhs>
constexpr auto operator||(const guard_t<Lhs>& lhs, const guard_t<Rhs>& rhs)
{
    return guard_t{detail::or_t{lhs, rhs}};
}

template<class Lhs, class Rhs>
constexpr auto operator||(const guard_t<Lhs>& lhs, const Rhs& rhs)
{
    return guard_t{detail::or_t{lhs, rhs}};
}

template<class Lhs, class Rhs>
constexpr auto operator||(const Lhs& lhs, const guard_t<Rhs>& rhs)
{
    return guard_t{detail::or_t{lhs, rhs}};
}

template<class Lhs, class Rhs>
constexpr auto operator!=(const guard_t<Lhs>& lhs, const guard_t<Rhs>& rhs)
{
    return guard_t{detail::xor_t{lhs, rhs}};
}

template<class Lhs, class Rhs>
constexpr auto operator!=(const guard_t<Lhs>& lhs, const Rhs& rhs)
{
    return guard_t{detail::xor_t{lhs, rhs}};
}

template<class Lhs, class Rhs>
constexpr auto operator!=(const Lhs& lhs, const guard_t<Rhs>& rhs)
{
    return guard_t{detail::xor_t{lhs, rhs}};
}

template<class Guard>
constexpr auto operator!(const guard_t<Guard>& grd)
{
    return guard_t{detail::not_t{grd}};
}

template<const auto& Guard>
inline constexpr auto guard = guard_t{Guard};

} //namespace

#endif
