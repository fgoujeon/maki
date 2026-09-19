// Copyright Florian Goujeon 2021 - 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
// Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_FORWARDER_STATE_MOLD_HPP
#define MAKI_FORWARDER_STATE_MOLD_HPP

#include "detail/friendly_impl.hpp"
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

        // Whatever
        using context_type = void;
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
class forwarder_state_mold
{
public:
    constexpr forwarder_state_mold() = default;

    forwarder_state_mold(const forwarder_state_mold&) = delete;

    forwarder_state_mold(forwarder_state_mold&&) = delete;

    ~forwarder_state_mold() = default;

    forwarder_state_mold& operator=(const forwarder_state_mold&) = delete;

    forwarder_state_mold& operator=(forwarder_state_mold&&) = delete;

    template<class Forwarder>
    constexpr auto forwarder()
    {
        using impl_t = detail::state_firewall_mold_impl<Forwarder>;
        return forwarder_state_mold<impl_t>{};
    }

private:
    MAKI_DETAIL_FRIENDLY_IMPL

    using impl_type = Impl;

    template<class Impl2>
    friend class forwarder_state_mold;

    template<class... Args>
    constexpr forwarder_state_mold(Args&&... args): impl_{std::forward<Args>(args)...}
    {
    }

    impl_type impl_;
};

#undef MAKI_DETAIL_STATE_CONF_RETURN_TYPE

#ifdef MAKI_DETAIL_DOXYGEN
forwarder_state_mold() -> forwarder_state_mold<IMPLEMENTATION_DETAIL>;
#else
forwarder_state_mold() -> forwarder_state_mold<detail::state_firewall_mold_impl<>>;
#endif

namespace detail
{
    template<class T>
    struct is_state_firewall_mold
    {
        static constexpr auto value = false;
    };

    template<class OptionSet>
    struct is_state_firewall_mold<forwarder_state_mold<OptionSet>>
    {
        static constexpr auto value = true;
    };

    template<class T>
    constexpr bool is_state_firewall_mold_v = is_state_firewall_mold<T>::value;
} // namespace detail

} // namespace maki

#endif
