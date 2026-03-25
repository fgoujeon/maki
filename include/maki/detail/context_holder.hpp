//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_CONTEXT_HOLDER_HPP
#define MAKI_DETAIL_CONTEXT_HOLDER_HPP

#include "../context.hpp"
#include "context_storage.hpp"
#include <type_traits>
#include <optional>
#include <utility>

namespace maki::detail
{

template<class T, context_storage Storage, auto Signature>
class context_holder
{
public:
    using storage_type = std::conditional_t
    <
        Storage == context_storage::plain,
        T,
        std::optional<T>
    >;

    template
    <
        class Machine,
        class... Args,
        auto Strg = Storage,
        auto Sig = Signature,
        std::enable_if_t<Strg == context_storage::plain && Sig == machine_context_signature::a, bool> = true
    >
    context_holder(Machine& /*mach*/, Args&&... args):
        ctx_{std::forward<Args>(args)...}
    {
    }

    template
    <
        class Machine,
        class... Args,
        auto Strg = Storage,
        auto Sig = Signature,
        std::enable_if_t<Strg == context_storage::plain && Sig == machine_context_signature::am, bool> = true
    >
    context_holder(Machine& mach, Args&&... args):
        ctx_{std::forward<Args>(args)..., mach}
    {
    }

    template
    <
        class Machine,
        class ParentContext,
        auto Strg = Storage,
        auto Sig = Signature,
        std::enable_if_t<Strg == context_storage::plain && Sig == state_context_signature::c, bool> = true
    >
    context_holder(Machine& /*mach*/, ParentContext& parent_ctx):
        ctx_{parent_ctx}
    {
    }

    template
    <
        class Machine,
        class ParentContext,
        auto Strg = Storage,
        auto Sig = Signature,
        std::enable_if_t<Strg == context_storage::plain && Sig == state_context_signature::cm, bool> = true
    >
    context_holder(Machine& mach, ParentContext& parent_ctx):
        ctx_{parent_ctx, mach}
    {
    }

    template
    <
        class Machine,
        class ParentContext,
        auto Strg = Storage,
        auto Sig = Signature,
        std::enable_if_t<Strg == context_storage::plain && Sig == state_context_signature::m, bool> = true
    >
    context_holder(Machine& mach, ParentContext& /*parent_ctx*/):
        ctx_{mach}
    {
    }

    template
    <
        class Machine,
        class ParentContext,
        auto Strg = Storage,
        auto Sig = Signature,
        std::enable_if_t<Strg == context_storage::optional || Sig == state_context_signature::v, bool> = true
    >
    context_holder(Machine& /*mach*/, ParentContext& /*parent_ctx*/)
    {
    }

    template<class Machine, class ParentContext>
    T& emplace
    (
        [[maybe_unused]] Machine& mach,
        [[maybe_unused]] ParentContext& parent_ctx
    )
    {
        if constexpr (Signature == state_context_signature::c)
        {
            return ctx_.emplace(parent_ctx);
        }
        else if constexpr (Signature == state_context_signature::cm)
        {
            return ctx_.emplace(parent_ctx, mach);
        }
        else if constexpr (Signature == state_context_signature::m)
        {
            return ctx_.emplace(mach);
        }
        else if constexpr (Signature == state_context_signature::v)
        {
            return ctx_.emplace();
        }
    }

    void reset()
    {
        ctx_.reset();
    }

    storage_type& get()
    {
        return ctx_;
    }

    const storage_type& get() const
    {
        return ctx_;
    }

    T& get_deep()
    {
        if constexpr (Storage == context_storage::plain)
        {
            return ctx_;
        }
        else
        {
            return ctx_.value();
        }
    }

    const T& get_deep() const
    {
        if constexpr (Storage == context_storage::plain)
        {
            return ctx_;
        }
        else
        {
            return ctx_.value();
        }
    }

private:
    storage_type ctx_;
};

} //namespace

#endif
