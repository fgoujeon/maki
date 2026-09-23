// Copyright Florian Goujeon 2021 - 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
// Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_FIREWALLED_STATE_CONF_HPP
#define MAKI_FIREWALLED_STATE_CONF_HPP

#include "context.hpp"
#include "detail/friendly_impl.hpp"
#include "detail/machine_conf_impl.hpp"
#include "detail/signature_macros.hpp"
#include "detail/tuple.hpp"
#include <utility>

namespace maki
{

#ifdef MAKI_DETAIL_DOXYGEN
#define MAKI_DETAIL_MACHINE_CONF_RETURN_TYPE \
    firewalled_state_conf<IMPLEMENTATION_DETAIL>
#else
#define MAKI_DETAIL_MACHINE_CONF_RETURN_TYPE auto
#endif

template<class Impl>
class firewalled_state_conf
{
public:
    constexpr firewalled_state_conf() = default;

    firewalled_state_conf(const firewalled_state_conf&) = delete;

    firewalled_state_conf(firewalled_state_conf&&) = delete;

    ~firewalled_state_conf() = default;

    firewalled_state_conf& operator=(const firewalled_state_conf&) = delete;

    firewalled_state_conf& operator=(firewalled_state_conf&&) = delete;

#define MAKI_DETAIL_X(signature) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    /** \
    @brief Sets the type of the context (see @ref \
    maki::machine_context_signature "signatures"). \
    */ \
    template<class Context> \
    [[nodiscard]] constexpr MAKI_DETAIL_MACHINE_CONF_RETURN_TYPE \
    context_##signature() const \
    { \
        return context<Context, machine_context_signature::signature>(); \
    }
    MAKI_DETAIL_MACHINE_CONTEXT_CONSTRUCTOR_SIGNATURES
#undef MAKI_DETAIL_X

    /**
    @brief Specifies the list of transition tables. One region per transition
    table is created.
    */
    template<class... TransitionTables>
    [[nodiscard]] constexpr MAKI_DETAIL_MACHINE_CONF_RETURN_TYPE
    transition_tables(const TransitionTables&... tables) const
    {
        const auto tpl = detail::tuple<TransitionTables...>{tables...};
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_transition_tables tpl
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END(firewalled_state_conf)
#undef MAKI_DETAIL_ARG_transition_tables
    }

private:
    MAKI_DETAIL_FRIENDLY_IMPL

    using impl_type = Impl;

    template<class Impl2>
    friend class firewalled_state_conf;

    template<class... Args>
    constexpr firewalled_state_conf(Args&&... args):
        impl_{std::forward<Args>(args)...}
    {
    }

    template<class Context2, auto ContextSig>
    [[nodiscard]] constexpr auto context() const {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_context_type detail::type<Context2>
#define MAKI_DETAIL_ARG_context_sig ContextSig
            MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END(firewalled_state_conf)
#undef MAKI_DETAIL_ARG_context_type
#undef MAKI_DETAIL_ARG_context_sig
    }

    impl_type impl_;
};

#undef MAKI_DETAIL_MACHINE_CONF_RETURN_TYPE

#ifdef MAKI_DETAIL_DOXYGEN
firewalled_state_conf() -> firewalled_state_conf<IMPLEMENTATION_DETAIL>;
#else
firewalled_state_conf() -> firewalled_state_conf<detail::machine_conf_impl<>>;
#endif

} // namespace maki

#endif
