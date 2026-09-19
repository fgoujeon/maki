// Copyright Florian Goujeon 2021 - 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
// Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_STATE_FIREWALL_MOLD_HPP
#define MAKI_STATE_FIREWALL_MOLD_HPP

#include "detail/friendly_impl.hpp"
#include <type_traits>
#include <utility>

namespace maki
{

namespace detail
{
    template<class Forwarder = void>
    struct state_firewall_mold_impl
    {
        using forwarder_type = Forwarder;

        struct void_context_type_holder
        {
            template<class Dummy = void>
            using type = void;
        };

        struct forwarder_context_type_holder
        {
            template<class Dummy = void>
            using type = typename Forwarder::context_type;
        };

        using context_type =
            typename std::conditional_t<
                std::is_void_v<Forwarder>,
                void_context_type_holder,
                forwarder_context_type_holder>::template type<>;
    };

    template<class Forwarder>
    constexpr bool is_composite(const state_firewall_mold_impl<Forwarder>& /*impl*/)
    {
        return true;
    }

    template<class Forwarder>
    constexpr bool is_forwarder(const state_firewall_mold_impl<Forwarder>& /*impl*/)
    {
        return true;
    }
}

template<class Impl>
class state_firewall_mold
{
public:
    constexpr state_firewall_mold() = default;

    state_firewall_mold(const state_firewall_mold&) = delete;

    state_firewall_mold(state_firewall_mold&&) = delete;

    ~state_firewall_mold() = default;

    state_firewall_mold& operator=(const state_firewall_mold&) = delete;

    state_firewall_mold& operator=(state_firewall_mold&&) = delete;

    template<class Forwarder>
    constexpr auto forwarder()
    {
        using impl_t = detail::state_firewall_mold_impl<Forwarder>;
        return state_firewall_mold<impl_t>{};
    }

private:
    MAKI_DETAIL_FRIENDLY_IMPL

    using impl_type = Impl;

    template<class Impl2>
    friend class state_firewall_mold;

    template<class... Args>
    constexpr state_firewall_mold(Args&&... args): impl_{std::forward<Args>(args)...}
    {
    }

    impl_type impl_;
};

#undef MAKI_DETAIL_STATE_CONF_RETURN_TYPE

#ifdef MAKI_DETAIL_DOXYGEN
state_firewall_mold() -> state_firewall_mold<IMPLEMENTATION_DETAIL>;
#else
state_firewall_mold() -> state_firewall_mold<detail::state_firewall_mold_impl<>>;
#endif

namespace detail
{
    template<class T>
    struct is_state_firewall_mold
    {
        static constexpr auto value = false;
    };

    template<class OptionSet>
    struct is_state_firewall_mold<state_firewall_mold<OptionSet>>
    {
        static constexpr auto value = true;
    };

    template<class T>
    constexpr bool is_state_firewall_mold_v = is_state_firewall_mold<T>::value;
} // namespace detail

} // namespace maki

#endif
